#pragma once
#include <SDL.h>
#undef main
#include <iostream>

//#include "Entity.h"
#include "Coordinator.h"
#include "VulkanEngine.h"



extern Coordinator gCoordinator;

class App
{
//Variables
private:
	std::unique_ptr<Entity> root;
	//std::unique_ptr<VulkanEngine> engine;
	//VulkanEngine* engine;
//Functions
private:
	void Input();
	void Update();
	void Render(std::shared_ptr<SDL_Renderer> context);
public:
	void start();
	void init();
	void destroy();
};

