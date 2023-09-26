#include "Entity.h"
#include <iostream>

Entity::Entity():id("Entity"+eCounter++)
{
}

Entity::Entity(const std::string id):id(id)
{
}

void Entity::addComponent(IComponent* component)
{
	this->components.push_back(component);
	component->setOwner(this);
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

void Entity::render(SDL_Renderer *context)
{
	//check render component is there
	for (IComponent *component : this->components) {
		if (typeid(component).name()=="SpriteComponent") {
			((SpriteComponent*)component)->render(context);
		}
	}
}

void Entity::addChild(Entity* child)
{
	this->children.push_back(child);
}
