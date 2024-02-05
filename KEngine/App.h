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
	std::shared_ptr<RenderSystem> renderSystem;
//Functions
private:
	void Input();
	void Update();
	void Render();
public:
	void start();
	void init();
	void destroy();
};

