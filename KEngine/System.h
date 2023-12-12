#pragma once

#include "EntityManager.h"
#include "Coordinator.h"
#include <set>

extern Coordinator gCoordinator;
class System
{
public:
	std::set<Entity> mEntities;
};