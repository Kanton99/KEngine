#include "SDL3/SDL_error.h"
#include "SDL3/SDL_vulkan.h"
#include "vkEngine/pipelineBuilder.hpp"
#include "vkEngine/swapchainBuilder.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <stdexcept>
#include <utility>
#include <vector>
#include <vkEngine/engine.hpp>
#include <vkEngine/utils.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace vkEngine {
vkEngine::vkEngine(std::shared_ptr<SDL_Window> window) :
	_window(window),
	_cleanupQueue(std::make_unique<CleanupQueue>()),
	_commandBufferHandler{},
	frameIndex{0} {
	this->framesInFlight.reserve(2);
}

void vkEngine::init() {
	this->_createInstance();
	this->_createSurface();
	this->_pickPhysicalDevice();
	this->_createLogicalDevice();
	this->_createSwapchain();
	this->_createImageView();
	this->_createGraphicsPipeline();
	this->_creteCommandBuffer();
	this->_createSyncObjects();
	std::cout << "Rendering engine initialization complete\n";
}
void vkEngine::draw() {
	this->_device.waitIdle();
	auto fenceResult = this->_device.waitForFences(this->framesInFlight[frameIndex].drawFence, vk::True, UINT32_MAX);
	if (fenceResult != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to wait for fence");
	}
	this->_device.resetFences(this->framesInFlight[frameIndex].drawFence);
	auto [result, imageIndex] = this->_device.acquireNextImageKHR(this->_swapchain, UINT32_MAX,
																  this->framesInFlight[frameIndex].presentComplete);
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to acquire next swapcain image");

	this->_recordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
	const vk::SubmitInfo submitInfo{.waitSemaphoreCount = 1,
									.pWaitSemaphores = &this->framesInFlight[frameIndex].presentComplete,
									.pWaitDstStageMask = &waitDestinationStageMask,
									.commandBufferCount = 1,
									.pCommandBuffers = &this->framesInFlight[frameIndex].commandBuffer,
									.signalSemaphoreCount = 1,
									.pSignalSemaphores = &this->framesInFlight[frameIndex].renderFinishedSemaphore};

	this->_graphicsQueue.submit(submitInfo, this->framesInFlight[frameIndex].drawFence);

	const vk::PresentInfoKHR presentInfo{.waitSemaphoreCount = 1,
										 .pWaitSemaphores = &this->framesInFlight[frameIndex].renderFinishedSemaphore,
										 .swapchainCount = 1,
										 .pSwapchains = &this->_swapchain,
										 .pImageIndices = &imageIndex};

	result = this->_graphicsQueue.presentKHR(presentInfo);
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present image");
	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
void vkEngine::cleanup() { this->_cleanupQueue->flush(); }

void vkEngine::_createInstance() {
	if constexpr (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1) {
		VULKAN_HPP_DEFAULT_DISPATCHER.init();
	}
	constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Hello triangle",
										  .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
										  .pEngineName = "vkEngine",
										  .engineVersion = VK_MAKE_VERSION(0, 0, 1),
										  .apiVersion = vk::ApiVersion14};

	uint32_t extensionCount;
	auto extensionsC{SDL_Vulkan_GetInstanceExtensions(&extensionCount)};
	if (!extensionsC) {
		std::cerr << "Failed to get window extensions: " << SDL_GetError() << std::endl;
	}
	std::vector extensions(extensionsC, extensionsC + extensionCount);
#ifndef NDEBUG
	std::println("Number of extensions: {}", extensionCount);
	for (int i = 0; i < extensionCount; i++) {
		auto extension{extensions[i]};
		std::println("[DEBUG] {}", extension);
	}

	this->_checkLayers(this->_validationLayers);
#endif
	this->_checkExtensions(extensions);

	vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
									  .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
									  .ppEnabledExtensionNames = extensions.data()};
	if constexpr (enableValidationLayers) {
		createInfo.ppEnabledLayerNames = this->_validationLayers.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(this->_validationLayers.size());
	}

	this->_instance = vk::createInstance(createInfo);
	if constexpr (VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1) {
		VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
	}
}

void vkEngine::_checkExtensions(std::vector<const char *> extensions) {
	std::cout << "Check required extensions support\n";
	auto extensionProperties = vk::enumerateInstanceExtensionProperties();
	auto unsupportedPropertyIt =
		std::ranges::find_if(extensions, [&extensionProperties](auto const &requiredExtension) {
			return std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) {
				return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
			});
		});
	if (unsupportedPropertyIt != extensions.end()) {
		throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
	}
}

void vkEngine::_checkLayers(std::vector<char const *> layers) {
	std::cout << "Check required validation layers support\n";
	auto instanceLayerProperties{vk::enumerateInstanceLayerProperties()};
	auto unsupportedLayerIt{std::ranges::find_if(layers, [&instanceLayerProperties](auto const &layer) {
		return std::ranges::none_of(instanceLayerProperties, [layer](auto const &layerProperty) {
			return strcmp(layerProperty.layerName, layer) == 0;
		});
	})};
	if (unsupportedLayerIt != layers.end()) {
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}
}

void vkEngine::_pickPhysicalDevice() {
	std::cout << "Selecting physical device to use\n";
	auto physicalDevices{this->_instance.enumeratePhysicalDevices()};
	if (physicalDevices.empty())
		throw std::runtime_error("Failed to find GPUs with vulkan suppoert!");

	std::multimap<int, vk::PhysicalDevice> candidates;
	for (const auto physicalDevice : physicalDevices) {
		auto deviceProperties{physicalDevice.getProperties()};
		auto deviceFeatures{physicalDevice.getFeatures()};
		uint32_t score = 0;

		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) // Big prefrence on discrete GPUs
			score += 1000;

		score += deviceProperties.limits.maxImageDimension2D;

		if (!(deviceFeatures.geometryShader && (deviceProperties.apiVersion >= vk::ApiVersion14)))
			continue;

		// Check for graphic queue family
		auto queueFamilies{physicalDevice.getQueueFamilyProperties()};
		bool supportsGraphics{std::ranges::any_of(queueFamilies, [](auto const &queueFamily) {
			return !!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics);
		})};
		if (!supportsGraphics)
			continue;

		// Check for required extensions
		std::vector<const char *> requiredDeviceExtensions = {vk::KHRSwapchainExtensionName,
															  vk::KHRShaderDrawParametersExtensionName,
															  vk::KHRSynchronization2ExtensionName};
		auto deviceExtensions{physicalDevice.enumerateDeviceExtensionProperties()};
		bool supportsAllRequiredGraphicExtensions{
			std::ranges::all_of(requiredDeviceExtensions, [&deviceExtensions](auto const &requiredDeviceExtension) {
				return std::ranges::any_of(deviceExtensions, [requiredDeviceExtension](auto const &deviceExtension) {
					return strcmp(deviceExtension.extensionName, requiredDeviceExtension) == 0;
				});
			})};

		if (!supportsAllRequiredGraphicExtensions)
			continue;

		// Check for required Features
		auto features{physicalDevice.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan14Features,
												  vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
												  vk::PhysicalDeviceSynchronization2Features>()};
		bool suppotsRequiredFeatures{
			features.get<vk::PhysicalDeviceVulkan14Features>().dynamicRenderingLocalRead &&
			features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState &&
			features.get<vk::PhysicalDeviceSynchronization2Features>().synchronization2};
		if (!suppotsRequiredFeatures)
			continue;

		candidates.insert(std::make_pair(score, physicalDevice));
	}

	if (!candidates.empty() && candidates.rbegin()->first > 0)
		this->_physicalDevice = candidates.rbegin()->second;
	else
		throw std::runtime_error("Failed to find a suitable GPU");
}

void vkEngine::_createLogicalDevice() {
	std::cout << "Creating logical device\n";

	// Setting up queue families creations
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties{this->_physicalDevice.getQueueFamilyProperties()};

	this->_graphicsQueueIndex = ~0;
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++) {
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			this->_physicalDevice.getSurfaceSupportKHR(qfpIndex, this->_surface)) {
			this->_graphicsQueueIndex = qfpIndex;
			break;
		}
	}

	float queuePriority{0.5f};
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{.queueFamilyIndex = this->_graphicsQueueIndex,
													.queueCount = 1,
													.pQueuePriorities = &queuePriority};

	// Getting required device features
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
					   vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
		featureChain = {
			{},													  // vk::PhysicalDeviceFeatures2 (empty for now)
			{.synchronization2 = true, .dynamicRendering = true}, // Enable dynamic rendering from Vulkan 1.3
			{.extendedDynamicState = true}						  // Enable extended dynamic state from the extension
		};
	std::vector<const char *> requiredDeviceExtension = {
		vk::KHRSwapchainExtensionName, vk::KHRShaderDrawParametersExtensionName, vk::KHRSynchronization2ExtensionName};

	// Creating logical device
	vk::DeviceCreateInfo deviceCreateInfo{.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
										  .queueCreateInfoCount = 1,
										  .pQueueCreateInfos = &deviceQueueCreateInfo,
										  .enabledExtensionCount =
											  static_cast<uint32_t>(requiredDeviceExtension.size()),
										  .ppEnabledExtensionNames = requiredDeviceExtension.data()};

	this->_device = this->_physicalDevice.createDevice(deviceCreateInfo);
	this->_cleanupQueue->pushFunction([&]() { this->_device.destroy(); });

	std::cout << "Getting graphics queue\n";
	this->_graphicsQueue = this->_device.getQueue(this->_graphicsQueueIndex, 0);
}

void vkEngine::_createSurface() {
	std::cout << "Creating window surface\n";
	VkSurfaceKHR baseSurface;
	if (!SDL_Vulkan_CreateSurface(this->_window.get(), static_cast<VkInstance>(this->_instance), nullptr,
								  &baseSurface)) {
		std::cerr << "Failed to create SDL surface: " << SDL_GetError() << std::endl;
	}
	this->_surface = vk::SurfaceKHR{baseSurface};
}

void vkEngine::_createSwapchain() {
	std::cout << "Creating swapchain\n";
	auto surfaceCapabilities = this->_physicalDevice.getSurfaceCapabilitiesKHR(this->_surface);
	auto builder = SwapchainBuilder{surfaceCapabilities};
	this->_swapchain = builder.chooseSwapExtent(*this->_window)
						   .chooseSwapMinImageCount()
						   .chooseSwapSurfaceFormat(this->_physicalDevice.getSurfaceFormatsKHR(this->_surface))
						   .buildSwapchain(this->_surface, this->_device);
	this->swapchainImages = this->_device.getSwapchainImagesKHR(this->_swapchain);
	this->_swapchainSurfaceFormat = builder.getFormat();
	this->_swapchianExtent = builder.getExtent();
}

void vkEngine::_createImageView() {
	vk::ImageViewCreateInfo imageViewInfo{
		.viewType = vk::ImageViewType::e2D,
		.format = this->_swapchainSurfaceFormat.format,
		.subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1}};
	imageViewInfo.setComponents({
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
	});

	for (auto const &image : this->swapchainImages) {
		imageViewInfo.image = image;
		auto imageView = this->_device.createImageView(imageViewInfo);
		_swapchainImageView.push_back(imageView);
	}
}

void vkEngine::_createGraphicsPipeline() {
	std::println("Creating graphics pipeline");
	PipelineBuilder builder{this->_device};
	this->_graphicsPipeline =
		builder.loadShaderCode("./resources/shaders/slang.spv")
			.createShaderModule()
			.createPipelineStage(vk::ShaderStageFlagBits::eVertex, "vertMain")
			.createPipelineStage(vk::ShaderStageFlagBits::eFragment, "fragMain")
			.setViewPortState({.extent = this->_swapchianExtent}, {.extent = this->_swapchianExtent})
			.build(this->_swapchainSurfaceFormat);
	std::println("Created Graphics pipeline");
}

void vkEngine::_creteCommandBuffer() {
	std::println("Creating graphics command buffer");
	this->_commandBufferHandler.createCommandPool(this->_device, this->_graphicsQueueIndex);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		this->framesInFlight[i].commandBuffer =
			_commandBufferHandler.allocateCommandBuffer(this->_device, vk::CommandBufferLevel::ePrimary);
	}

	std::println("Created graphics command buffer");
}
void vkEngine::_recordCommandBuffer(uint32_t imageIndex) {
	vk::CommandBufferBeginInfo beginInfo{};
	auto beginResult = this->framesInFlight[frameIndex].commandBuffer.begin(&beginInfo);
	if (beginResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to begin recording command buffer");
	transitionImageLayout(
		this->swapchainImages[imageIndex], this->framesInFlight[frameIndex].commandBuffer, vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput);

	vk::ClearValue clearColor{vk::ClearColorValue(0.f, 0.f, 0.f, 1.f)};
	vk::RenderingAttachmentInfo attachmentInfo{.imageView{this->_swapchainImageView[imageIndex]},
											   .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
											   .loadOp = vk::AttachmentLoadOp::eClear,
											   .storeOp = vk::AttachmentStoreOp::eStore,
											   .clearValue{clearColor}};

	vk::RenderingInfo renderingIndo{.renderArea{.offset{0, 0}, .extent{this->_swapchianExtent}},
									.layerCount = 1,
									.colorAttachmentCount = 1,
									.pColorAttachments = &attachmentInfo};
	this->framesInFlight[frameIndex].commandBuffer.beginRendering(renderingIndo);

	this->framesInFlight[frameIndex].commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
																this->_graphicsPipeline);

	this->framesInFlight[frameIndex].commandBuffer.setViewport(
		0, vk::Viewport{0.f, 0.f, static_cast<float>(this->_swapchianExtent.width),
						static_cast<float>(this->_swapchianExtent.height)});
	this->framesInFlight[frameIndex].commandBuffer.setScissor(0,
															  vk::Rect2D{vk::Offset2D{0, 0}, this->_swapchianExtent});

	this->framesInFlight[frameIndex].commandBuffer.draw(3, 1, 0, 0);
	this->framesInFlight[frameIndex].commandBuffer.endRendering();

	transitionImageLayout(this->swapchainImages[imageIndex], this->framesInFlight[frameIndex].commandBuffer,
						  vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
						  vk::AccessFlagBits2::eColorAttachmentWrite, {},
						  vk::PipelineStageFlagBits2::eColorAttachmentOutput,
						  vk::PipelineStageFlagBits2::eBottomOfPipe);

	this->framesInFlight[frameIndex].commandBuffer.end();
}

void vkEngine::_createSyncObjects() {
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		this->framesInFlight[i].presentComplete = this->_device.createSemaphore({});
		this->framesInFlight[i].renderFinishedSemaphore = this->_device.createSemaphore({});
		this->framesInFlight[i].drawFence = this->_device.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
	}
	this->_cleanupQueue->pushFunction([&]() {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			this->framesInFlight[i].cleanup(this->_device);
		}
	});
}
} // namespace vkEngine
