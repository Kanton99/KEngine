#pragma once
#include "Entity.h"

class IComponent
{
public:
	virtual void update();

	void setOwner(Entity *owner);
protected:
	Entity *owner;
};

