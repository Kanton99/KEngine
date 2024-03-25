#pragma once
#include "System.h"
#include <memory>
#include "VulkanEngine.h"
#include "Engine.h"
#include <SDL.h>

class RenderSystem : public System
{

	//std::unique_ptr<VulkanEngine> engine;
	std::unique_ptr<VulkanEngine> engine;

public: //TODO do more secure
	SDL_Window* window;

public:
	void draw();
	RenderSystem();

	void loadModel(Entity entity, std::string modelPath);

	void loadTexture(Entity entity, std::string texturePath);

	void cleanUp();

};

