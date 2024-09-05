#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <VkBootstrap.h>
class vkEngine
{
public:
	void init();
private:
#pragma region init
	VkInstance _instance;
	SDL_Window* _window;
	VkDevice* _device;
	VkPhysicalDevice* _physDevice;
#pragma endregion

};
