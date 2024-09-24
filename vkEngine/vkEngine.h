#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_extension_inspection.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <memory>



class vkEngine
{
public:
	static vkEngine* get(SDL_Window* window);
	void init();

	void cleanup();

private:
	vkEngine(SDL_Window* window);

	void _init_instance();
#ifndef NDEBUG
	void _init_debug_messenger();
#endif // !NDEBUG
	uint32_t _init_devices();
	void _init_command_pool(uint32_t queue_family_index);

	void _allocate_command_buffer(std::vector<vk::raii::CommandBuffer> buffer_list);
	void _record_command_buffer();
private:
	static vkEngine* _engine;
#pragma region init
	vk::raii::Instance _instance = nullptr;
	SDL_Window* _window;
	vk::raii::SurfaceKHR _surface = nullptr;
#ifndef NDEBUG
	vk::raii::DebugUtilsMessengerEXT _debug_messanger = nullptr;
#endif // !DEBUG

	vk::raii::Device _device = nullptr;
	vk::raii::PhysicalDevice _phys_device = nullptr;

	vk::raii::Queue _graphics_queue = nullptr;
#pragma endregion

#pragma region CommandBuffers
	vk::raii::CommandPool _command_pool = nullptr;

	vk::raii::CommandBuffer _graphics_command_buffer = nullptr;
#pragma endregion

#pragma region SWAPCHAIN
	vk::raii::SwapchainKHR _graphic_swapchain = nullptr;
#pragma endregion


};
