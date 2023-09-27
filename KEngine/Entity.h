#pragma once
#include <vector>
#include "IComponent.h"
#include <string>
#include "Components.h"

int eCounter = 0;

class Entity
{
public:
	std::string id;

protected:
	std::vector<IComponent*> components;
	std::vector<Entity*> children;

public:
	Entity();
	Entity(const std::string id);
	void addComponent(IComponent *component);
	void removeComponent(IComponent* component);
	void updatesComponents();

	void update();
	void render(SDL_Renderer *context);
	
	void addChild(Entity* child);


};

