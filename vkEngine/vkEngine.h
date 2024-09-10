#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
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

	bool _dynamicStorageCreated;
#pragma region init
	vk::Instance _instance;
	SDL_Window* _window;
	vk::SurfaceKHR _surface;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT _debug_messanger;
#endif // !DEBUG

	vk::Device _device;
	vk::PhysicalDevice _physDevice;

	vk::Queue _graphics_queue;
#pragma endregion

};
