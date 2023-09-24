#include "Entity.h"
#include <iostream>

void Entity::addComponent(IComponent* component)
{
	this->components.push_back(component);
}

void Entity::removeComponent(IComponent* component)
{
	int position=0;
	for (; position < this->components.size(); position++) {
		if (this->components[position] == component) {
			this->components.erase(this->components.begin() + position);
			return;
		}
	}
	std::cout << "Component not found in Entity\n";
	return;
}

void Entity::updatesComponents()
{
	for(IComponent *component : this->components)
	{
		component->update();
	}
}

void Entity::update()
{
	for (Entity* child : this->children) {
		child->update();
	}
}
