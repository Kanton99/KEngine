#pragma once
#include <vulkan/vulkan.h>
#include <vector>
class VulkanEngine
{
private:
	VkInstance instance;
	bool checkValidationSupport();
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
private:
	std::vector<const char*> getRequiredExtensions();

	void setupDebugMessenger();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);


public:
	bool _isInitialized{ false };
	int _frameNumber{ 0 };

	VkExtent2D _windowExtent{ 1700,900 };

	struct SDL_Window* _window{ nullptr };

	void init();

	void cleanup();

	void draw();

	void run();

	void createInstance();

	void pickPhysicalDevice();
};

