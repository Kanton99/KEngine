#pragma once
#include <SDL.h>
#undef main
#include <iostream>

//#include "Entity.h"
#include "Coordinator.h"
#include "VulkanEngine.h"
#include "RenderSystem.h"



extern Coordinator gCoordinator;

class App
{
//Variables
private:
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

