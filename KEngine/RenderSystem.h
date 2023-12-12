#pragma once
#include "System.h"

class RenderSystem : public System
{
	void draw();

	std::unique_ptr<VulkanEngine> engine;

public:
	RenderSystem();
	~RenderSystem();
};

