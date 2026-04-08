#include "SDL3/SDL_error.h"
#include "SDL3/SDL_vulkan.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <vkEngine/engine.hpp>

namespace vkEngine {
vkEngine::vkEngine(std::shared_ptr<SDL_Window> window) :
	_window(window),
	_cleanupQueue(std::make_unique<CleanupQueue>()) {}

void vkEngine::init() { this->_createInstance(); }
void vkEngine::draw() {}
void vkEngine::cleanup() { this->_cleanupQueue->flush(); }

void vkEngine::_createInstance() {
	constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Hello triangle",
										  .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
										  .pEngineName = "vkEngine",
										  .engineVersion = VK_MAKE_VERSION(0, 0, 1),
										  .apiVersion = vk::ApiVersion14};

	uint32_t extensionCount;
	auto extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
	if (!extensions) {
		std::cerr << "Failed to get window extensions: " << SDL_GetError() << std::endl;
	}
	std::cout << "Number of extensions: " << extensionCount << std::endl;
	for (int i = 0; i < extensionCount; i++) {
		auto extension = extensions[i];
		std::cout << extension << std::endl;
	}
	vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
									  .enabledExtensionCount = extensionCount,
									  .ppEnabledExtensionNames = extensions};

	this->_instance = vk::createInstance(createInfo);
}
} // namespace vkEngine
