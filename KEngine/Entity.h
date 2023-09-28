#pragma once
#include <vector>
#include <string>
#include "IComponent.h"
#include <SDL.h>
#include <memory>

class IComponent;
class Entity
{
public:
	std::string id;

protected:
	std::vector<std::unique_ptr<IComponent>> components;
	std::vector<std::unique_ptr<Entity>> children;

	static int eCounter;

public:
	Entity();
	Entity(const std::string id);
	~Entity();

	void addComponent(std::unique_ptr<IComponent> component);
	void removeComponent(std::unique_ptr<IComponent> component);
	void updatesComponents();

	void update();
	void render(SDL_Renderer *context);
	
	void addChild(std::unique_ptr<Entity> child);


};

