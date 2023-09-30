#pragma once
#include <vulkan/vulkan.h>
class VulkanEngine
{
private:
	VkInstance instance;

public:
	bool _isInitialized{ false };
	int _frameNumber{ 0 };

	VkExtent2D _windowExtent{ 1700,900 };

	struct SDL_Window* _window{ nullptr };

	void init();

	void cleanup();

	void draw();

	void run();

	bool checkValidationSupport();

	void createInstance();
};

