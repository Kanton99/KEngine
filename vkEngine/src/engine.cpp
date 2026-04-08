#include "SDL3/SDL_error.h"
#include "SDL3/SDL_vulkan.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vkEngine/engine.hpp>
#include <vulkan/vulkan_funcs.hpp>
#include <vulkan/vulkan_hpp_macros.hpp>

namespace vkEngine {
vkEngine::vkEngine(std::shared_ptr<SDL_Window> window) :
	_window(window),
	_cleanupQueue(std::make_unique<CleanupQueue>()) {}

void vkEngine::init() {
	this->_createInstance();
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
	auto extensionsC = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
	if (!extensionsC) {
		std::cerr << "Failed to get window extensions: " << SDL_GetError() << std::endl;
	}
	std::vector extensions(extensionsC, extensionsC + extensionCount);
#ifndef NDEBUG
	std::cout << "Number of extensions: " << extensionCount << std::endl;
	for (int i = 0; i < extensionCount; i++) {
		auto extension = extensions[i];
		std::cout << extension << std::endl;
	}

	this->_checkLayers(this->_validationLayers);
#endif
	this->_checkExtensions(extensions);

	vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
									  .enabledExtensionCount = extensionCount,
									  .ppEnabledExtensionNames = extensions.data()};

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
	auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
	auto unsupportedLayerIt = std::ranges::find_if(layers, [&instanceLayerProperties](auto const &layer) {
		return std::ranges::none_of(instanceLayerProperties, [layer](auto const &layerProperty) {
			return strcmp(layerProperty.layerName, layer) == 0;
		});
	});
	if (unsupportedLayerIt != layers.end()) {
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}
}

void vkEngine::_createDevice() {}
} // namespace vkEngine
