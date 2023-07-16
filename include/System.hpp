#pragma once

#include "Registry.hpp"

namespace au::framework {

class System {
public:
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void OnPrepare(Registry& registry) = 0;
    virtual void OnShutdown(Registry& registry) = 0;

    virtual void OnPreTick(Registry& registry, float delta) = 0;
    virtual void OnPostTick(Registry& registry, float delta) = 0;

    virtual void OnTick(Registry& registry, float delta) = 0;

    template <typename T>
    using RegistryBasedContainer = std::unordered_map<Registry*, T>;
};

}
