#include "Engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include "vk_initializers.h"
#include "vk_types.h"
#include "VkBootstrap/VkBootstrap.h"
#include "fmt/include/fmt/core.h"
#include "vk_images.h"

#include <chrono>
#include <thread>

#ifdef DEBUG
    constexpr bool useValidationLayers = false
#else
    constexpr bool useValidationlayers = true;
#endif // DEBUG

/// <summary>
/// Initialize 
/// </summary>
void Engine::init() {
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);

	init_vulkan();

	init_swapchain();

	init_commands();

	init_sync_structures();

	//everything went fine
	_isInitialized = true;
}

void Engine::init_vulkan() {
	vkb::InstanceBuilder builder;

#ifdef DEBUG 
	auto inst_ret = builder.set_app_name("Vulkan Engine Debug")
		.request_validation_layers(useValidationlayers)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();
#else
	auto inst_ret = builder.set_app_name("Vulkan Engine Release")
		.request_validation_layers(useValidationlayers)
		.require_api_version(1, 3, 0)
		.build();
#endif 

	vkb::Instance vkb_inst = inst_ret.value();

	_instance = vkb_inst.instance;

#ifdef DEBUG
	_debug_messenger = vkb_inst.debug_messenger;
#endif // DEBUG

	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	VkPhysicalDeviceVulkan13Features features{};
	features.dynamicRendering = true; //Skip renderpasses and framebuffers
	features.synchronization2 = true; //Use new synchronization functions

	VkPhysicalDeviceVulkan12Features features12{};
	features12.bufferDeviceAddress = true; //Direct access to GPU memory
	features12.descriptorIndexing = true; //Use of bindless textures

	//Select GPU
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicaldevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_surface(_surface)
		.select()
		.value();

	vkb::DeviceBuilder deviceBuilder{ physicaldevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	_device = vkbDevice.device;
	_chosenGPU = physicaldevice.physical_device;

	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void Engine::cleanup() {
	if (_isInitialized) {
		vkDeviceWaitIdle(_device);

		for (int i = 0; i < FRAME_OVERLAP; i++) { 
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr); 

			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);
		}

		destroy_swapchain();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);

#ifdef DEBUG
		vkb::destroy_debug_utils_messenger(_instace, _debug_messenger);
#endif // DEBUG
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}


void Engine::draw() {
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 10000000));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex));
	
	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	//make the swapchain image into writable mode before rendering
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	//make a clear-color frame from frame number
	VkClearColorValue clearValue;
	float flash = abs(sin(_frameNumber / 120.f));
	clearValue = { {0.f,0.f,flash,1.f} };

	auto clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	//make the swapchain image into presentable mode
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer
	VK_CHECK(vkEndCommandBuffer(cmd));

	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	auto cmdInfo = vkinit::command_buffer_submit_info(cmd);

	auto waitInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame()._swapchainSemaphore);
	auto signalInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame()._renderSemaphore);

	auto submit = vkinit::submit_info(&cmdInfo, &signalInfo, &waitInfo);

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));


	//prepare present
	// this will put the image we just rendered into the visible window
	// we want to wait on the _renderSemaphore for that,
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	_frameNumber++;

}
#pragma region SWAPCHAIN
void Engine::create_swapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder(_chosenGPU, _device, _surface);
	_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	_swapchainExtent = vkbSwapchain.extent;
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void Engine::init_swapchain() {
	create_swapchain(_windowExtent.width, _windowExtent.height);
}

void Engine::destroy_swapchain() {
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);

	for (int i = 0; i < _swapchainImageViews.size(); i++) vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
}
#pragma endregion

#pragma region COMMAND_BUFFERS
void Engine::init_commands() {
	/*VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _graphicsQueueFamily;*/
	auto commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		/*VkCommandBufferAllocateInfo cmdAllcInfo = {};
		cmdAllcInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllcInfo.pNext = nullptr;
		cmdAllcInfo.commandPool = _frames[i]._commandPool;
		cmdAllcInfo.commandBufferCount = 1;
		cmdAllcInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;*/
		auto cmdAllcInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllcInfo, &_frames[i]._mainCommandBuffer));
	}
}
#pragma endregion

#pragma region SYNCHRONIZATION
void Engine::init_sync_structures() {
	VkFenceCreateInfo fenceCreteInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateFence(_device, &fenceCreteInfo, nullptr, &_frames[i]._renderFence));

		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
	}


}
#pragma endregion

