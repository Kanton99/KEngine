#include "SDL3/SDL_error.h"
#include "SDL3/SDL_vulkan.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <vkEngine/engine.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace vkEngine {
vkEngine::vkEngine(std::shared_ptr<SDL_Window> window) :
	_window(window),
	_cleanupQueue(std::make_unique<CleanupQueue>()) {}

void vkEngine::init() {
	this->_createInstance();
	this->_createSurface();
	this->_pickPhysicalDevice();
	this->_createLogicalDevice();
	std::cout << "Rendering engine initialization complete\n";
}
void vkEngine::draw() {}
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
	std::cout << "Number of extensions: " << extensionCount << std::endl;
	for (int i = 0; i < extensionCount; i++) {
		auto extension{extensions[i]};
		std::cout << extension << std::endl;
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
		std::vector<const char *> requiredDeviceExtensions = {vk::KHRSwapchainExtensionName};
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
												  vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()};
		bool suppotsRequiredFeatures{
			features.get<vk::PhysicalDeviceVulkan14Features>().dynamicRenderingLocalRead &&
			features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState};
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
	auto graphicQueueFamilyProperty{std::ranges::find_if(queueFamilyProperties, [](auto const &qfp) {
		return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
	})};
	auto graphicsIndex{static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicQueueFamilyProperty))};
	float queuePriority{0.5f};
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{.queueFamilyIndex = graphicsIndex,
													.queueCount = 1,
													.pQueuePriorities = &queuePriority};

	// Getting required device features
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
					   vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
		featureChain = {
			{},							   // vk::PhysicalDeviceFeatures2 (empty for now)
			{.dynamicRendering = true},	   // Enable dynamic rendering from Vulkan 1.3
			{.extendedDynamicState = true} // Enable extended dynamic state from the extension
		};
	std::vector<const char *> requiredDeviceExtension = {vk::KHRSwapchainExtensionName};

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
	this->_graphicsQueue = this->_device.getQueue(graphicsIndex, 0);
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
} // namespace vkEngine
