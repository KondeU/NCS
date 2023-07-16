#pragma once

#include <unordered_map>
#include "Node.hpp"

#define COMPONENT(name) \
static constexpr au::ncs::Name name##ComponentName = #name;            \
static constexpr au::ncs::Uuid name##ComponentUuid = AU_CT_UUID(name); \
struct name##Component final

namespace au::ncs {

struct ComponentStorage {
    virtual void* GetComponent(Node node) = 0;
    virtual void* AddComponent(Node node) = 0;
    virtual bool RemoveComponent(Node node) = 0;
};

// Build-in storage.
template <typename Component>
struct ComponentBuffer : ComponentStorage {
    std::unordered_map<Node, Component> components;

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
};

}
