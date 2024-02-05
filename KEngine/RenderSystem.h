#pragma once
#include "System.h"
#include <memory>
#include "VulkanEngine.h"

class RenderSystem : public System
{

	std::unique_ptr<VulkanEngine> engine;

public:
	void draw();
	RenderSystem();

	void loadModel(Entity entity, std::string modelPath);

	void cleanUp();

};

