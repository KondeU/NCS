#pragma once

#include <functional>
#include "Viewer.hpp"

namespace au::ncs {

class Registry final {
public:
    template <typename Component>
    bool RegisterComponentStorage(ComponentStorage* storage)
    {
        #if defined (DEBUG) || defined (_DEBUG)
        debugComponentNameHashMap[Component::ComponentName] = Component::ComponentUuid;
        #endif
        return storages.emplace(Component::ComponentUuid, storage).second; // Failed if exist.
    }

    template <typename Component>
    ComponentStorage* UnregisterComponentStorage()
    {
        #if defined (DEBUG) || defined (_DEBUG)
        debugComponentNameHashMap.erase(Component::ComponentName);
        #endif
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return nullptr; // Failed because storage is no exist.
        }
        auto output = storage->second;
        storages.erase(storage);
        return output;
    }

    template <typename Component>
    Component* GetComponent(Node node)
    {
        if (node.IsInvalid()) {
            return nullptr; // Failed because input node is invalid.
        }
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return nullptr; // Failed because storage is not yet registered.
        }
        return static_cast<Component*>(storage->second->GetComponent(node));
    }

    template <typename Component>
    Component* AddComponent(Node node)
    {
        if (node.IsInvalid()) {
            return nullptr; // Failed because input node is invalid.
        }
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return nullptr; // Failed because storage is not yet registered.
        }
        return static_cast<Component*>(storage->second->AddComponent(node));
    }

    template <typename Component>
    bool RemoveComponent(Node entity)
    {
        if (entity.IsInvalid()) {
            return false; // Failed because input node is invalid.
        }
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return false; // Failed because storage is not yet registered.
        }
        return storage->second->RemoveComponent(entity);
    }

    Node CreateNode()
    {
        return Node(gid++);
    }

    void DestroyNode(Node node)
    {
        for (auto& [uuid, storage] : storages) {
            storage->RemoveComponent(node);
        }
    }

    template <typename Component>
    bool ForEach(std::function<void(Node, Component&)> process)
    {
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return false; // Failed because storage is not yet registered.
        }
        if (auto buffer = dynamic_cast<ComponentBuffer<Component>*>(storage->second)) {
            for (auto& [node, component] : buffer->components) {
                process(node, component);
            }
        }
        return true;
    }

private:
    #if defined (DEBUG) || defined (_DEBUG)
    std::unordered_map<std::string, Uuid> debugComponentNameHashMap;
    #endif
    std::unordered_map<Uuid, ComponentStorage*> storages;
    Node::Id gid = 10; // The first ten IDs are reserved.
};

}
