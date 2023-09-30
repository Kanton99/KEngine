#include "VulkanEngine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLAyers = true;
#endif

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

bool VulkanEngine::checkValidationSupport() {
	unsigned int layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void VulkanEngine::createInstance() {
	if (enableValidationLAyers && !checkValidationSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "test app";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
	appInfo.pEngineName = "kEngine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//TODO 
	//check for extension support

	unsigned int extCount = 0;
	SDL_Vulkan_GetInstanceExtensions(this->_window, &extCount, nullptr);
	const int n = extCount;
	const char** extNames = new const char*[extCount];
	if (!SDL_Vulkan_GetInstanceExtensions(this->_window, &extCount, extNames)) {
		throw std::runtime_error("Not enough extensions loaded!");
	}

	createInfo.enabledExtensionCount = extCount;
	createInfo.ppEnabledExtensionNames = extNames;


	if (enableValidationLAyers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateInstance(&createInfo, nullptr, &instance)!=VK_SUCCESS) throw std::runtime_error("failed to create instance!");
}

void VulkanEngine::init() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_window = SDL_CreateWindow(
		"kEngine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);

	createInstance();

	_isInitialized = true;
}

void VulkanEngine::cleanup() {
	if (_isInitialized) {
		vkDestroyInstance(this->instance, nullptr);
		SDL_DestroyWindow(_window);
		SDL_Quit();
	}

}

void VulkanEngine::draw() {
	//TODO
}

void VulkanEngine::run() {
	SDL_Event e;
	bool bQuit = false;

	while (!bQuit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) bQuit = true;
		}

		draw();
	}
}