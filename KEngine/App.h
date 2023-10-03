#pragma once
#include <SDL.h>
#undef main
#include <iostream>

#include "Entity.h"
#include "Entity2D.h"
#include "VulkanEngine.h"

class App
{
//Variables
private:
	std::unique_ptr<Entity> root;
	std::unique_ptr<VulkanEngine> engine;

//Functions
private:
	void Input();
	void Update();
	void Render(std::shared_ptr<SDL_Renderer> context);
public:
	void start();
	void init();
};

