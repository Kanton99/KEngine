#pragma once
#pragma once
#include <vector>
#include "IComponent.h"

class Entity
{
private:
	std::vector<IComponent*> components;
	std::vector<Entity*> children;

public:
	void addComponent(IComponent *component);
	void removeComponent(IComponent* component);
	void updatesComponents();

	void update();

};

