#pragma once

#include <unordered_map>
#include "Platform.hpp"
#include "Node.hpp"

#define DEFINE_COMPONENT(name)                                  \
static constexpr au::ncs::Name ComponentName = #name;           \
static constexpr au::ncs::Uuid ComponentUuid = AU_CT_UUID(#name);

namespace au::ncs {

struct ComponentStorage {
    virtual Uuid GetType() const = 0;
    virtual void* GetComponent(Node node) = 0;
    virtual void* AddComponent(Node node) = 0;
    virtual bool RemoveComponent(Node node) = 0;
    virtual std::vector<Node> GetNodes() const = 0;
    virtual size_t GetSize() const = 0;
    virtual void TrimMemory() = 0;
};

template <typename Component>
struct ComponentLooper {
    virtual void ForEach(const std::function<void(Node, Component&)>& process) = 0;
};

template <typename Component>
struct ComponentBuffer : ComponentStorage, ComponentLooper<Component> {
    struct SkipFields final {
        uint32_t fields = 0;

        inline bool IsAllUsed() const noexcept
        {
            return fields == ~0U;
        }

        inline bool IsAllFree() const noexcept
        {
            return fields == 0U;
        }

        inline void MarkUsedBit(int bit) noexcept
        {
            fields |= 1 << bit;
        }

        inline void MarkFreeBit(int bit) noexcept
        {
            fields &= ~(1 << bit);
        }

        inline int FindFreeBit() const
        {
            return FindZeroBit(fields); // Return -1 if free bit is not found.
        }

        static inline int FindZeroBit(uint32_t bits)
        {
            return (bits == ~0U) ? -1 :
            [](uint32_t inverted) {
                #if defined(_MSC_VER)
                unsigned long index = 32; // _BitScanForward output parameter.
                return (_BitScanForward(&index, inverted) > 0) ? static_cast<int>(index) : -1;
                #elif defined(__GNUC__) || defined(__clang__)
                return __builtin_ctz(inverted);
                #else // Software implementation.
                for (int i = 0; i < 32; i++) {
                    if ((inverted & (1U << i)) != 0) {
                        return i;
                    }
                }
                #endif
                return -1;
            }(~bits);
        }
    };

    struct Unit final {
        Node node;
        uint8_t data[sizeof(Component)];
        ct_assert(sizeof(Node) == sizeof(Uuid)); // Unit is aligned to sizeof(Uuid).

        inline Component* Construct(Node componentBelongedNode)
        {
            node = componentBelongedNode;
            return (new (ComponentPointer()) Component);
        }

        inline void Destruct()
        {
            ComponentPointer()->~Component();
            #if defined(DEBUG) || defined(_DEBUG)
            node = INVALID_NODE;
            #endif
        }

        inline Component* ComponentPointer() noexcept
        {
            return reinterpret_cast<Component*>(data);
        }
    };

    struct Block final {
        SkipFields skipFields;
        Unit units[32]{}; // skipFields.fields is uint32_t, 32 bits.

        inline bool IsFull() const noexcept
        {
            return skipFields.IsAllUsed();
        }

        inline bool IsEmpty() const noexcept
        {
            return skipFields.IsAllFree();
        }

        inline Unit* FindFree()
        {
            int index = skipFields.FindFreeBit();
            if (index < 0) {
                return nullptr; // Not found free bit.
            }
            skipFields.MarkUsedBit(index);
            return &units[index];
        }

        inline bool TryFree(Unit* unit)
        {
            int index = unit - &units[0]; // Elements diff count.
            if (!(static_cast<unsigned int>(index) < 32U)) {
                return false; // Not in this block.
            }
            skipFields.MarkFreeBit(index);
            return true;
        }
    };

    std::list<Block> blocks; // Container.
    std::unordered_map<Node, Unit*> finder;

    Uuid GetType() const override
    {
        return Component::ComponentUuid;
    }

    void* GetComponent(Node node) override
    {
        auto iter = finder.find(node);
        if (iter == finder.end()) {
            return nullptr;
        }
        auto unit = iter->second;
        return unit->ComponentPointer();
    }

    void* AddComponent(Node node) override
    {
        if (auto component = GetComponent(node)) {
            return component;
        }

        auto block = blocks.begin();
        while (block != blocks.end()) {
            if (block->IsFull()) {
                block++;
            } else {
                break;
            }
        }
        if (block == blocks.end()) {
            blocks.emplace_back();
            block = std::prev(blocks.end());
        }

        auto unit = block->FindFree();
        rt_assert_if (unit) {
            return nullptr; // Internal logic error, block is not full but find free failed.
        }
        finder[node] = unit;
        return unit->Construct(node);
    }

    bool RemoveComponent(Node node) override
    {
        auto iter = finder.find(node);
        if (iter == finder.end()) {
            return false;
        }
        auto unit = iter->second;
        finder.erase(iter);

        unit->Destruct();
        bool free = false;
        for (auto& block : blocks) {
            free = block.TryFree(unit);
            if (free) {
                break;
            }
        }
        rt_assert_if (free) {
            return false; // Internal logic error, can not find this unit in all block.
        }
        return true;
    }

    std::vector<Node> GetNodes() const override
    {
        std::vector<Node> nodes;
        nodes.reserve(finder.size());
        for (auto& block : blocks) {
            uint32_t fields = block.skipFields.fields;
            for (int i = 0; (i < 32) && (fields != 0); i++) {
                if ((fields & 1U) != 0) {
                    nodes.emplace_back(block.units[i].node);
                }
                fields >>= 1;
            }
        }
        return nodes;
    }

    size_t GetSize() const override
    {
        return finder.size();
    }

    void TrimMemory() override
    {
        auto iter = blocks.begin();
        while (iter != blocks.end()) {
            if (iter->IsEmpty()) {
                iter = blocks.erase(iter);
            } else {
                iter++;
            }
        }
        finder.rehash(0);
    }

    void ForEach(const std::function<void(Node, Component&)>& process) override
    {
        for (auto& block : blocks) {
            uint32_t fields = block.skipFields.fields;
            for (int i = 0; (i < 32) && (fields != 0); i++) {
                if ((fields & 1U) != 0) {
                    process(block.units[i].node,
                        *(block.units[i].ComponentPointer()));
                }
                fields >>= 1;
            }
        }
    }
};

template <typename Component>
struct UnorderedComponentBuffer : ComponentStorage, ComponentLooper<Component> {
    std::unordered_map<Node, Component> components;

    Uuid GetType() const override
    {
        return Component::ComponentUuid;
    }

    void* GetComponent(Node node) override
    {
        auto component = components.find(node);
        if (component == components.end()) {
            return nullptr;
        }
        return &component->second;
    }

    void* AddComponent(Node node) override
    {
        return &components[node];
    }

    bool RemoveComponent(Node node) override
    {
        return (components.erase(node) > 0);
    }

    std::vector<Node> GetNodes() const override
    {
        std::vector<Node> nodes;
        nodes.reserve(components.size());
        for (const auto& component : components) {
            nodes.emplace_back(component.first);
        }
        return nodes;
    }

    size_t GetSize() const override
    {
        return components.size();
    }

    void TrimMemory() override
    {
        components.rehash(0);
    }

    void ForEach(const std::function<void(Node, Component&)>& process) override
    {
        for (auto& [node, component] : components) {
            process(node, component);
        }
    }
};

}
