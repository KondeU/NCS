#pragma once

#include <unordered_map>
#include "Node.hpp"

#define DEFINE_COMPONENT(name)                                 \
static constexpr au::ncs::Name ComponentName = #name;          \
static constexpr au::ncs::Uuid ComponentUuid = AU_CT_UUID(name);

namespace au::ncs {

struct ComponentStorage {
    virtual Uuid GetType() const = 0;
    virtual void* GetComponent(Node node) = 0;
    virtual void* AddComponent(Node node) = 0;
    virtual bool RemoveComponent(Node node) = 0;
    virtual std::vector<Node> GetNodes() const = 0;
};

template <typename Component>
struct ComponentBuffer : ComponentStorage {
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
};

}
