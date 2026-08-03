#ifndef VK_ENGINE
#define VK_ENGINE
#include "cleanupStruct.hpp"
#include "vkEngine/commandBufferHandler.hpp"
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <vkEngine/framesInFlight.hpp>
#include <vulkan/vulkan_handles.hpp>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkEngine {
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;
class vkEngine {
  public:
	vkEngine(std::shared_ptr<SDL_Window> window);
	vkEngine(vkEngine &&) = delete;
	vkEngine(const vkEngine &) = delete;
	vkEngine &operator=(vkEngine &&) = delete;
	vkEngine &operator=(const vkEngine &) = delete;
	~vkEngine() = default;

	void init();

	void draw();

	void cleanup();

  private:
	void _createInstance();
	void _checkExtensions(std::vector<const char *> extensions);
	void _checkLayers(std::vector<char const *> layers);
	void _pickPhysicalDevice();
	void _createLogicalDevice();
	void _createSurface();
	void _createSwapchain();
	void _createImageView();
	void _createGraphicsPipeline();
	void _creteCommandBuffer();
	void _recordCommandBuffer(uint32_t imageIndex);
	void _createSyncObjects();

  private:
	vk::Instance _instance = nullptr;
	std::shared_ptr<SDL_Window> _window;
	const std::vector<char const *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

	vk::PhysicalDevice _physicalDevice;
	vk::Device _device;
	vk::Queue _graphicsQueue;
	uint32_t _graphicsQueueIndex;

	vk::SurfaceKHR _surface;

	vk::SwapchainKHR _swapchain;
	std::vector<vk::Image> swapchainImages;
	vk::SurfaceFormatKHR _swapchainSurfaceFormat;
	vk::Extent2D _swapchianExtent;
	std::vector<vk::ImageView> _swapchainImageView;

	vk::Pipeline _graphicsPipeline;

	CommandBufferHandler _commandBufferHandler;
	vk::CommandBuffer _graphicsCommandBuffer;

	// Synchronization objects
	vk::Semaphore presentCompleteSemaphore = nullptr;
	vk::Semaphore renderFinishedSemaphore = nullptr;
	vk::Fence drawFence = nullptr;

	uint32_t frameIndex;
	std::vector<FrameInFlight> framesInFlight;

	std::unique_ptr<CleanupQueue> _cleanupQueue;
};
} // namespace vkEngine
#endif
