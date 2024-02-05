#pragma once

#include "EntityManager.hpp"
#include <set>

class System
{
public:
	std::set<Entity> mEntities;
};