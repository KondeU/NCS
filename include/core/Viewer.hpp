#pragma once

#include <functional>
#include <unordered_set>
#include "Component.hpp"

namespace au::ncs {

class ViewFilter {
public:
    virtual void AddNode(Node node) = 0;
    virtual void RemoveNode(Node node) = 0;
    virtual void ClearAllNodes() = 0;

    const std::unordered_set<Uuid>& GetComponentTypes() const
    {
        return filter;
    }

    bool HasType(Uuid componentTypeUuid) const
    {
        return (filter.find(componentTypeUuid) != filter.end());
    }

protected:
    // This class only could be inherited by the Viewer class.
    explicit ViewFilter(std::unordered_set<Uuid> componentTypes) : filter(componentTypes) {}

private:
    const std::unordered_set<Uuid> filter;
};

class ViewBinder {
public:
    virtual void BindViewerToRegistry(ViewFilter& viewer) = 0;
    virtual void UnbindViewerFromRegistry(ViewFilter& viewer) = 0;

protected:
    // This class only could be inherited by the Registry class.
    ViewBinder() = default;
};

template <typename ...Components>
class Viewer : public ViewFilter {
public:
    // The number of Viewer will affect performance.
    explicit Viewer(ViewBinder& binder) : binder(binder),
        ViewFilter({ Components::ComponentUuid... })
    {
        binder.BindViewerToRegistry(*this);
    }

    virtual ~Viewer()
    {
        binder.UnbindViewerFromRegistry(*this);
    }

    template <typename Component>
    bool HasComponentType() const
    {
        return HasType(Component::ComponentUuid);
    }

    void AddNode(Node node) override
    {
        nodes.insert(node);
    }

    void RemoveNode(Node node) override
    {
        nodes.erase(node);
    }

    void ClearAllNodes() override
    {
        nodes.clear();
    }

    void ForEach(std::function<void(Node)> process)
    {
        for (const auto& node : nodes) {
            process(node);
        }
    }

    // Forbidden copying.
    Viewer(const Viewer&) = delete;
    Viewer& operator=(const Viewer&) = delete;
    // Forbidden moving.
    Viewer(Viewer&&) = delete;
    Viewer& operator=(Viewer&&) = delete;

private:
    ViewBinder& binder; // Registry
    std::unordered_set<Node> nodes;
};

}
