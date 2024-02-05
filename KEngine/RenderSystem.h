#pragma once
#include "System.h"
#include <memory>
#include "VulkanEngine.h"

class RenderSystem : public System
{
	void draw();

	std::unique_ptr<VulkanEngine> engine;

public:
	RenderSystem();

	void loadModel(Entity entity, std::string modelPath);

};

