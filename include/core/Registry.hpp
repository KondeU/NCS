#pragma once

#include <algorithm>
#include "Viewer.hpp"

namespace au::ncs {

template <typename Relation>
class Registry : public Relation, public ViewBinder {
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
        auto component = static_cast<Component*>(storage->second->GetComponent(node));
        return component;
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
        auto component = static_cast<Component*>(storage->second->AddComponent(node));
        for (auto viewer : viewers) {
            if (!viewer->HasComponentType(Component::ComponentUuid)) {
                continue; // Next viewer.
            }
            bool addNodeToViewer = true;
            for (const auto& type : viewer.GetComponentTypes()) {
                if (type == Component::ComponentUuid) {
                    continue; // Skip current component type.
                }
                if (auto typeStorage = storages.find(type); typeStorage != storages.end()) {
                    if (!typeStorage->second->GetComponent(node)) {
                        addNodeToViewer = false;
                        break;
                    }
                }
            }
            if (addNodeToViewer) {
                viewer->AddNode(node);
            }
        }
        return component;
    }

    template <typename Component>
    bool RemoveComponent(Node node)
    {
        if (node.IsInvalid()) {
            return false; // Failed because input node is invalid.
        }
        auto storage = storages.find(Component::ComponentUuid);
        if (storage == storages.end()) {
            return false; // Failed because storage is not yet registered.
        }
        bool removed = storage->second->RemoveComponent(node);
        for (auto viewer : viewers) {
            if (viewer->HasComponentType(Component::ComponentUuid)) {
                viewer->RemoveNode(node);
            }
        }
        return removed;
    }

    Node CreateNode()
    {
        Node node(gid++);
        if (!Relation::OnNodeCreate(node, this)) {
            return INVALID_NODE;
        }
        return node;
    }

    bool DestroyNode(Node node)
    {
        bool success = Relation::OnNodeDestroy(node, this);
        for (auto& [uuid, storage] : storages) {
            success &= storage->RemoveComponent(node);
        }
        return success; // Destruction may not be clean if return false.
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
        } else {          // Internal logic error maybe occur if run here,
            return false; // because UUID and ComponentBuffer correspond one-to-one.
        }
        return true;
    }

    void BindViewerToRegistry(ViewFilter& viewer) override
    {
        std::vector<Node> pending;
        for (const auto& type : viewer.GetComponentTypes()) {
            auto storage = storages.find(type);
            if (storage == storages.end()) {
                pending.clear();
                break;
            }
            if (pending.empty()) {
                pending = storage->second->GetNodes();
            } else {
                std::vector<Node> intersection(pending.size());
                auto nodes = storage->second->GetNodes();
                auto end = std::set_intersection(
                    pending.begin(), pending.end(),
                    nodes.begin(), nodes.end(),
                    intersection.begin());
                intersection.resize(end - intersection.begin());
                pending = intersection; // Intersect the nodes of each component.
            }
            if (pending.empty()) {
                break;
            }
        }
        viewer.ClearAllNodes();
        for (const auto& node : pending) {
            viewer.AddNode(node);
        }
        viewers.insert(&viewer);
    }

    void UnbindViewerFromRegistry(ViewFilter& viewer) override
    {
        viewers.erase(&viewer);
    }

    // Internal node ID:
    static constexpr Node INVALID_NODE      = Node(0);
    static constexpr Node DEFAULT_ROOT_NODE = Node(1);

    // Forbidden copying.
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

private:
    #if defined (DEBUG) || defined (_DEBUG)
    std::unordered_map<std::string, Uuid> debugComponentNameHashMap;
    #endif
    std::unordered_map<Uuid, ComponentStorage*> storages;
    Node::Id gid = 10; // The first ten IDs are reserved.
    std::unordered_set<ViewFilter*> viewers;
};

}
