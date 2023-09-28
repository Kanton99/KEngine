#include "Entity.h"
#include "SpriteComponent.h"
#include "Transform.h"
#include <iostream>

Entity::Entity():id("Entity"+eCounter++)
{
}

Entity::Entity(const std::string id):id(id)
{
}

void Entity::addComponent(std::unique_ptr<IComponent> component)
{
	component->setOwner(this);
	this->components.push_back(std::move(component));
}

Entity::~Entity()
{
	for (std::unique_ptr<Entity>&child : this->children) {
		child.reset();
		child.release();
	}

	for (auto &component : this->components) {
		auto* c = component.release();
		delete c;
		component.reset();
	}
}

void Entity::removeComponent(std::unique_ptr<IComponent> component)
{
	for (int position = 0; position < this->components.size(); position++) {
		if (this->components[position] == component) {
			this->components.erase(this->components.begin() + position);
			component.release();
			return;
		}
	}
	std::cout << "Component not found in Entity\n";
	return;
}

void Entity::updatesComponents()
{
}

void Entity::update()
{
	updatesComponents();
	for (std::unique_ptr<Entity>& child : this->children) {
		child->update();
	}
}

void Entity::render(SDL_Renderer *context)
{
	//check render component is there
	for (auto& component : this->components) {
		if (component->type & SPRITE_COMPONENT) {
			dynamic_cast<SpriteComponent*>(component.get())->render(context);
		}
	}

	for (std::unique_ptr<Entity> &child : children) {
		child->render(context);
	}
}

void Entity::addChild(std::unique_ptr<Entity> child)
{
	this->children.push_back(std::move(child));
}
