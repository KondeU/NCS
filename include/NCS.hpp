#pragma once

#include "core/System.hpp"
#include "core/Relation.hpp"

namespace au::ncs {

template <typename Relation>
class Scene {
public:
	Registry<Relation>& GetRegistry()
	{
		return registry;
	}

	// TODO: RegisterSystem, UnregisterSystem

	void Tick()
	{
		for (const auto& system : systems) {
			// TODO
		}
	}

	// TODO: Viewer
	

private:
	Registry<Relation> registry;
	std::vector<System<Relation>*> systems;
};

using TreeScene = Scene<TreeRelation>;
using UnorderedTreeScene = Scene<UnorderedTreeRelation>;

}