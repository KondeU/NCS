#pragma once

#include "Registry.hpp"

namespace au::ncs {

class System {
public:
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void OnPrepare(Registry& registry) = 0;
    virtual void OnShutdown(Registry& registry) = 0;

    virtual void OnPrepTick(Registry& registry, float delta) = 0;
    virtual void OnProcTick(Registry& registry, float delta) = 0;
    virtual void OnPostTick(Registry& registry, float delta) = 0;
};

}
