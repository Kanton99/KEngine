#pragma once

#include "EntityManager.h"
#include <set>
class System
{
public:
	std::set<Entity> mEntities;
};