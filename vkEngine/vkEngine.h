#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <VkBootstrap.h>
#include <memory>

class vkEngine
{
public:
	static vkEngine* get(SDL_Window* window);
	void init();

	void cleanup();

private:
	vkEngine(SDL_Window* window);
private:
	static vkEngine* _engine;
#pragma region init
	VkInstance _instance;
	SDL_Window* _window;
	VkSurfaceKHR _surface;
	VkDebugUtilsMessengerEXT _debug_messanger;

	VkDevice _device;
	VkPhysicalDevice _physDevice;

	VkQueue _graohics_queue;
#pragma endregion

};
