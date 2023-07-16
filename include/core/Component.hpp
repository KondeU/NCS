#pragma once

#include <unordered_map>
#include "Entity.hpp"

#define META_COMPONENT(name) struct name##MetaComponent
#define DATA_COMPONENT(name) struct name##DataComponent

#define COMPONENT_IMPLEMENTATION(name, pod)                    \
struct name##Component {                                       \
    name##MetaComponent meta;                                  \
    name##DataComponent data;                                  \
    static_assert((std::is_trivial<name##MetaComponent>::value \
        && std::is_standard_layout<name##MetaComponent>::value \
        ) || !(pod), "MetaComponent must be POD!");            \
    static_assert((std::is_trivial<name##DataComponent>::value \
        && std::is_standard_layout<name##DataComponent>::value \
        ) || !(pod), "DataComponent must be POD!");            \
}

#define COMPONENT(name) COMPONENT_IMPLEMENTATION(name, false)

namespace au::framework {

struct ComponentStorage {
    virtual void* GetComponent(Entity entity) = 0;
    virtual void* AddComponent(Entity entity) = 0;
    virtual bool RemoveComponent(Entity entity) = 0;
};

template <typename Component>
struct ComponentBuffer : ComponentStorage {
    std::unordered_map<Entity, Component> components;

    void* GetComponent(Entity entity) override
    {
        auto component = components.find(entity);
        if (component == components.end()) {
            return nullptr;
        }
        return &component->second;
    }

    void* AddComponent(Entity entity) override
    {
        return &components[entity];
    }

    bool RemoveComponent(Entity entity) override
    {
        return (components.erase(entity) > 0);
    }
};

}
