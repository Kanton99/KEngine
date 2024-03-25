#pragma once

#include "vk_types.h"
#include "DeletionQueue.h"

struct FrameData {
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	DeletionQueue _deletionQueue;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine
{
public://Members
	bool _isInitialized{ false };
	uint32_t _frameNumber{ 0 };
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 800, 450 };

	struct SDL_Window* _window{ nullptr };

#pragma region ENGINE_INIT
	VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface;// Vulkan window surface
#pragma endregion

#pragma region SWAPCHAIN_INIT
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;
#pragma endregion

#pragma region COMMAND_BUFFERS
	FrameData _frames[FRAME_OVERLAP];
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
#pragma endregion

	DeletionQueue _mainDeletionQueue;

	VmaAllocator _allocator;
#pragma region Draw Resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;
#pragma endregion


private://Members


public: //Methods
	static VulkanEngine& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();
	void draw_background(VkCommandBuffer cmd);

	//run main loop
	void run();

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

private://Methods

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();

	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
};

