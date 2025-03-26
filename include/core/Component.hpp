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
};

template <typename Component>
struct ComponentBuffer : ComponentStorage {
    static constexpr size_t EachBlockFieldsCount = 4;
    static constexpr size_t EachFieldUnitsCount = 32;
    static constexpr size_t EachBlockUnitsCount = EachFieldUnitsCount * EachBlockFieldsCount;

    struct SkipFields final {
        uint32_t fields[EachBlockFieldsCount]{};

        inline bool IsAllUsed() const noexcept
        {
            return ((fields[0] & fields[1] & fields[2] & fields[3]) == ~0U);
        }

        inline bool IsAllFree() const noexcept
        {
            return ((fields[0] | fields[1] | fields[2] | fields[3]) == 0U);
        }

        inline void MarkUsedBit(int field, int bit) noexcept
        {
            fields[field] |= 1 << bit;
        }

        inline void MarkFreeBit(int field, int bit) noexcept
        {
            fields[field] &= ~(1 << bit);
        }

        inline bool FindFreeBit(int& field, int& bit)
        {
            for (field = 0; field < 4; field++) { // EachBlockFieldsCount = 4
                bit = FindZeroBit(fields[field]);
                if (bit >= 0) {
                    return true;
                }
            }
            return false;
        }

        static inline static int FindZeroBit(uint32_t bits)
        {
            return (bits == ~0U) ? -1 :
            [](uint32_t inverted) {
                #if defined(_MSC_VER)
                unsigned long index = 32; // _BitScanForward output parameter.
                return (_BitScanForward(&index, inverted) > 0) ? static_cast<int>(index) : -1;
                #elif defined(__GNUC__) || defined(__clang__)
                return __builtin_ctz(inverted);
                #else // Software implementation.
                for (int i = 0; i < 32; i++) { // EachFieldUnitsCount = 32
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

        inline Component* Construct()
        {
            return (new (ComponentPointer()) Component);
        }

        inline void Destruct()
        {
            ComponentPointer()->~Component();
        }

        inline Component* ComponentPointer() noexcept
        {
            return static_cast<Component*>(data);
        }
    };

    struct Block final {
        SkipFields skipFields;
        Unit units[EachBlockUnitsCount]{};

        inline bool IsFull() const noexcept
        {
            return fields.IsAllUsed();
        }

        inline bool IsEmpty() const noexcept
        {
            return fields.IsAllFree();
        }

        inline int FindUnitIndex(const Unit* unit)
        {
            //TODO
        }

        inline void ForEach()
        {
            //TODO
        }
    };


    std::list<Block> blocks; // Container.
    std::unordered_map<Node, Unit*> finder;

    void TrimBlocks()
    {
        auto iter = blocks.begin() + 1;
        while (iter != blocks.end()) {
            if (*iter.IsEmpty()) {
                iter = blocks.erase(iter);
            } else {
                iter++;
            }
        }
    }

    ComponentBuffer()
    {
        blocks.emplace_back();
    }

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
        return component->second;
    }

    void* AddComponent(Node node) override
    {
        auto& component = components[node];
        if (component) {
            return component;
        }

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
};

template <typename Component>
struct UnorderedComponentBuffer : ComponentStorage {
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
};

}
