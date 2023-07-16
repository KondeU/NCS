#pragma once

#include <typeinfo>
#include "Component.hpp"

namespace au::ncs {

class Registry final {
public:
    using TypeHash = decltype(typeid(void).hash_code());

    template <typename Component>
    bool RegistComponentStorage(ComponentStorage* storage)
    {
        #if defined (DEBUG) || defined (_DEBUG)
        componentNameHashMap[typeid(Component).name()] = typeid(Component).hash_code();
        #endif
        AU_LOG_I(TAG, "Regist component storage, type name is `%s`, type hash is `%x`.",
            typeid(Component).name(), typeid(Component).hash_code());
        return storages.emplace(typeid(Component).hash_code(), storage).second;
    }

    template <typename Component>
    ComponentStorage* UnregistComponentStorage()
    {
        #if defined (DEBUG) || defined (_DEBUG)
        componentNameHashMap.erase(typeid(Component).name());
        #endif
        AU_LOG_I(TAG, "Unregist component storage, type name is `%s`, type hash is `%x`.",
            typeid(Component).name(), typeid(Component).hash_code());
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            return nullptr;
        }
        auto output = storage->second;
        storages.erase(storage);
        return output;
    }

    template <typename Component>
    Component* GetComponent(Node entity)
    {
        if (entity.IsInvalid()) {
            AU_LOG_RETN_E(TAG, "Get component failed, input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            AU_LOG_RETN_E(TAG, "Get component failed, the storage for current component is not "
                "yet registered! Current component type name is `%s`, type hash is `%x`.",
                typeid(Component).name(), typeid(Component).hash_code());
        }
        return static_cast<Component*>(storage->second->GetComponent(entity));
    }

    template <typename Component>
    Component* AddComponent(Node entity)
    {
        if (entity.IsInvalid()) {
            AU_LOG_RETN_E(TAG, "Add component failed, input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            AU_LOG_RETN_E(TAG, "Add component failed, the storage for current component is not "
                "yet registered! Current component type name is `%s`, type hash is `%x`.",
                typeid(Component).name(), typeid(Component).hash_code());
        }
        return static_cast<Component*>(storage->second->AddComponent(entity));
    }

    template <typename Component>
    bool RemoveComponent(Node entity)
    {
        if (entity.IsInvalid()) {
            AU_LOG_RETF_E(TAG, "Remove component failed, input entity is invalid!");
        }
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            AU_LOG_RETN_E(TAG, "Remove component failed, the storage for current component is not "
                "yet registered! Current component type name is `%s`, type hash is `%x`.",
                typeid(Component).name(), typeid(Component).hash_code());
        }
        return storage->second->RemoveComponent(entity);
    }

    Node CreateNode()
    {
        return Node(gid++);
    }

    void DestroyNode(Node entity)
    {
        for (auto& [hash, storage] : storages) {
            storage->RemoveComponent(entity);
        }
    }

    template <typename Component>
    bool ForEach(std::function<void(Node, Component&)> process)
    {
        auto storage = storages.find(typeid(Component).hash_code());
        if (storage == storages.end()) {
            AU_LOG_RETF_E(TAG, "ForEach failed, the storage for current component is not "
                "yet registered! Current component type name is `%s`, type hash is `%x`.",
                typeid(Component).name(), typeid(Component).hash_code());
        }
        if (auto buffer = dynamic_cast<ComponentBuffer<Component>*>(storage->second)) {
            for (auto& [entity, component] : buffer->components) {
                process(entity, component);
            }
        }
        return true;
    }

private:
    #if defined (DEBUG) || defined (_DEBUG)
    std::unordered_map<std::string, TypeHash> componentNameHashMap;
    #endif
    std::unordered_map<TypeHash, ComponentStorage*> storages;
    Node::Id gid = 10; // The first ten IDs are reserved.
};

}
