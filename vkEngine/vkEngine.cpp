#include "vkEngine.h"
#include <iostream>

#ifdef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

vkEngine* vkEngine::_engine = nullptr;

vkEngine* vkEngine::get(SDL_Window* window)
{
	if (!_engine) {
		_engine = new vkEngine(window);
	}
	return _engine;
}

void vkEngine::init()
{
#ifdef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif // VULKAN_HPP_DISPATCH_LOADER_DYNAMIC


	vk::ApplicationInfo app_info("KEngine", 1, "vkEngine", VK_API_VERSION_1_3);

	const char* instance_extension_names = {
		"VK_EXT_debug_utils"
	};


	vk::InstanceCreateInfo instance_info({}, &app_info, {},instance_extension_names);
	this->_instance = vk::createInstance(instance_info);

#ifdef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init();


	this->_phys_device = this->_instance.enumeratePhysicalDevices().front();

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = this->_phys_device.getQueueFamilyProperties();

	auto property_iterator = std::find_if(queueFamilyProperties.begin(),
		queueFamilyProperties.end(),
		[](vk::QueueFamilyProperties const& qfp) {
			return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
		}
	);

	size_t graphics_queue_family_index = std::distance(queueFamilyProperties.begin(), property_iterator);
	
	assert(graphics_queue_family_index < queueFamilyProperties.size());

	float queue_priotity = 0.f;
	vk::DeviceQueueCreateInfo device_queue_create_info(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(graphics_queue_family_index), 1, &queue_priotity);
	this->_device = this->_phys_device.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), device_queue_create_info));

#ifdef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init();

#ifndef NDEBUG

	std::vector<vk::ExtensionProperties> props = vk::enumerateInstanceExtensionProperties();

	auto propertyIterator = std::find_if(
		props.begin(), props.end(), [](vk::ExtensionProperties const& ep) { return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0; });
	if (propertyIterator == props.end())
	{
		std::cout << "Something went very wrong, cannot find " << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << " extension" << std::endl;
		exit(1);
	}

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo{
		{},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		nullptr
	};

	this->_debug_messanger = this->_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerInfo, nullptr);
#endif // !DEBUG

}

void vkEngine::cleanup()
{

#ifndef NDEBUG
	this->_instance.destroyDebugUtilsMessengerEXT(this->_debug_messanger);
#endif // !NDEBUG
	this->_device.destroy();
	this->_instance.destroy();
}

vkEngine::vkEngine(SDL_Window* window):
	_window(window),
	_dynamicStorageCreated(false)
{
}
