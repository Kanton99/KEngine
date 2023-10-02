#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
struct QueueFanilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<unsigned int> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class VulkanEngine
{
private:
	VkInstance instance;
	bool checkValidationSupport();
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
private:

	std::vector<const char*> getRequiredExtensions();

	void setupDebugMessenger();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void createSurface();

	void createInstance();

	void pickPhysicalDevice();

	void createLogicalDevice();

	QueueFanilyIndices findQueueFamilies(VkPhysicalDevice device);

	bool isDeviceSuitable(VkPhysicalDevice device);

public:
	bool _isInitialized{ false };
	int _frameNumber{ 0 };

	VkExtent2D _windowExtent{ 1700,900 };

	struct SDL_Window* _window{ nullptr };

	void init();

	void cleanup();

	void draw();

	void run();

};

