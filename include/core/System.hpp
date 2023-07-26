#pragma once

#include "Registry.hpp"

namespace au::ncs {

template <typename Relation>
class System {
public:
    virtual ~System() = default;

    virtual void OnPrepare(Registry<Relation>& registry) = 0;
    virtual void OnShutdown(Registry<Relation>& registry) = 0;

    virtual void OnPrepTick(Registry<Relation>& registry, float delta) = 0;
    virtual void OnProcTick(Registry<Relation>& registry, float delta) = 0;
    virtual void OnPostTick(Registry<Relation>& registry, float delta) = 0;
};

}
