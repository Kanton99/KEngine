#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_extension_inspection.hpp>
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

	bool _dynamicStorageCreated;
#pragma region init
	vk::Instance _instance;
	SDL_Window* _window;
	vk::SurfaceKHR _surface;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT _debug_messanger;
#endif // !DEBUG

	vk::Device _device;
	vk::PhysicalDevice _phys_device;

	vk::Queue _graphics_queue;
#pragma endregion

};
