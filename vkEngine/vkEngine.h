#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_extension_inspection.hpp>
#include <SDL.h>
#include <memory>



class vkEngine
{
public:
	static vkEngine* get(SDL_Window* window);
	void init();

	void cleanup();

private:
	vkEngine(SDL_Window* window);

	void _init_command_pool(int queue_family_index);

	void _allocate_command_buffer(std::vector<vk::CommandBuffer> buffer_list);
	void _record_command_buffer();
private:
	static vkEngine* _engine;
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

#pragma region CommandBuffers
	vk::CommandPool _command_pool;

	vk::CommandBuffer _graphics_command_buffer;
#pragma endregion

};
