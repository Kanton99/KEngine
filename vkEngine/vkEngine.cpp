#include "vkEngine.h"
#include <iostream>

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
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
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif // VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	_init_instance();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(INSTANCE);

#ifndef NDEBUG
	_init_debug_messenger();
#endif

	auto queue_family_index = _init_devices();
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(INSTANCE);
	_init_command_pool(queue_family_index);

}

void vkEngine::cleanup()
{
}

vkEngine::vkEngine(SDL_Window* window):
	_window(window)
{
}

void vkEngine::_init_instance()
{
	vk::raii::Context context;

	vk::ApplicationInfo app_info{
		.pApplicationName = "KEngine",
		.pEngineName = "vkEngine",
		.apiVersion = VK_API_VERSION_1_3,
	};

	vk::InstanceCreateInfo instance_info{
		.pApplicationInfo = &app_info,
	};

	unsigned int ext_count;
	SDL_Vulkan_GetInstanceExtensions(this->_window, &ext_count, nullptr);
	std::vector<const char*> instance_extensions(ext_count);
	SDL_Vulkan_GetInstanceExtensions(this->_window, &ext_count, instance_extensions.data());
	
#ifndef NDEBUG
	instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	instance_info.setPEnabledExtensionNames(instance_extensions);

	this->_instance = vk::raii::Instance(context, instance_info);
	std::cout << "Instance created\n";
}

#ifndef NDEBUG
void vkEngine::_init_debug_messenger() {

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	vk::DebugUtilsMessageTypeFlagsEXT    messageTypeFlags(
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | 
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	//vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT({}, severityFlags, messageTypeFlags, &debugMessageFunc);
	vk::DebugUtilsMessengerCreateInfoEXT debug_create_info{
		.messageSeverity = severityFlags,
		.messageType = messageTypeFlags
	};

	this->_debug_messanger = vk::raii::DebugUtilsMessengerEXT(this->_instance, debug_create_info);
	std::cout << "Debug messenger created\n";
}
#endif // !NDEBUG

uint32_t vkEngine::_init_devices()
{
	auto phys_device_enum = this->_instance.enumeratePhysicalDevices();
	//CHANGE TO SELECT BEST
	this->_phys_device = phys_device_enum.front();

	auto phys_properties = this->_phys_device.getProperties();

	auto queue_phys_properties = this->_phys_device.getQueueFamilyProperties();
	uint32_t queue_family_index = 0;
	for (const auto queue_property : queue_phys_properties) {
		if (queue_property.queueFlags & vk::QueueFlagBits::eGraphics) {
			break;
		}
		queue_family_index++;
	}

	float queue_priority = 0.f;
	vk::DeviceQueueCreateInfo queue_info{
		.flags = {},
		.queueFamilyIndex = queue_family_index,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority
	};

	vk::DeviceCreateInfo device_info{
		.flags = {},
		.pQueueCreateInfos = &queue_info,

	};
	this->_device = this->_phys_device.createDevice(device_info);

	std::cout << "Device created\n";
	return queue_family_index;
}

void vkEngine::_init_command_pool(uint32_t queue_family_index)
{
	vk::CommandPoolCreateInfo pool_info{
		.flags = {},
		.queueFamilyIndex = queue_family_index
	};

	this->_command_pool = this->_device.createCommandPool(pool_info);
	std::cout << "Command pool created\n";
}

void vkEngine::_allocate_command_buffer(std::vector<vk::raii::CommandBuffer> buffer_list)
{
}

void vkEngine::_record_command_buffer()
{
}



