#pragma once
#include "Entity.h"

class Entity;

enum ComponentType {
	COMPONENT = 1,
	SPRITE_COMPONENT = 1 << 2,
	TRANSFORM = 1 << 1
};

class IComponent
 {
public:
	virtual void update();

	void setOwner(Entity *owner);

	ComponentType type = COMPONENT;
protected:
	Entity *owner;
};