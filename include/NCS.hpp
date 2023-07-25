#pragma once

#include <chrono>
#include "core/System.hpp"
#include "core/Relation.hpp"

namespace au::ncs {

template <typename Relation>
class Scene {
public:
    Scene()
    {
        timestamp = std::chrono::steady_clock::now();
    }

    Registry<Relation>& GetRegistry()
    {
        return registry;
    }

    Node GetDefaultRootNode() const
    {
        return Registry<Relation>::DEFAULT_ROOT_NODE;
    }

    bool RegisterSystem(System<Relation>& system)
    {
        auto iter = std::find(systems.begin(), systems.end(), &system);
        if (iter != systems.end()) {
            return false; // System has been registered.
        }
        system.OnPrepare();
        systems.emplace_back(&system);
        return true;
    }

    bool UnregisterSystem(System<Relation>& system)
    {
        auto iter = std::find(systems.begin(), systems.end(), &system);
        if (iter == systems.end()) {
            return false; // System has not been registered.
        }
        systems.erase(iter);
        system.OnShutdown();
        return true;
    }

    void Tick()
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - timestamp);
        float delta = static_cast<float>(static_cast<double>(duration.count()) / 1000.0);

        for (const auto& system : systems) {
            system->OnPrepTick(registry, delta);
        }
        for (const auto& system : systems) {
            system->OnProcTick(registry, delta);
        }
        for (const auto& system : systems) {
            system->OnPostTick(registry, delta);
        }
    }

private:
    Registry<Relation> registry;
    std::vector<System<Relation>*> systems;
    std::chrono::steady_clock::time_point timestamp;
};

using TreeScene = Scene<TreeRelation>;
using TreeSystem = System<TreeRelation>;
using TreeRegistry = Registry<TreeRelation>;
using UnorderedTreeScene = Scene<UnorderedTreeRelation>;
using UnorderedTreeSystem = System<UnorderedTreeRelation>;
using UnorderedTreeRegistry = Registry<UnorderedTreeRelation>;

}
