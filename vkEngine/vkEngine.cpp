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

	vk::ApplicationInfo app_info{
		.pApplicationName = "KEngine",
		.applicationVersion = 0,
		.pEngineName = "vkEngine",
		.apiVersion = VK_API_VERSION_1_3
	};

	std::vector<const char*> instance_extension_names = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	//vk::InstanceCreateInfo instance_info({}, &app_info, {},instance_extension_names);
	vk::InstanceCreateInfo instance_info{
		.flags = {},
		.pApplicationInfo = &app_info
	};
	instance_info.setPEnabledExtensionNames(instance_extension_names);
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
	//vk::DeviceQueueCreateInfo device_queue_create_info(vk::DeviceQueueCreateFlags{}, static_cast<uint32_t>(graphics_queue_family_index), 1, &queue_priotity);
	vk::DeviceQueueCreateInfo device_queue_create_info{
		.flags = {},
		.queueFamilyIndex = static_cast<uint32_t>(graphics_queue_family_index),
		.queueCount = 1,
		.pQueuePriorities = &queue_priotity
	};
	this->_device = this->_phys_device.createDevice(vk::DeviceCreateInfo{.flags=vk::DeviceCreateFlags(), .pQueueCreateInfos=&device_queue_create_info});

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
		.flags = {},
		.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		.pfnUserCallback = nullptr
	};

	this->_debug_messanger = this->_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerInfo, nullptr);
#endif // CREATE DEBUG UTILS MESSENGER

	this->_init_command_pool(graphics_queue_family_index);
	this->_allocate_command_buffer(std::vector<vk::CommandBuffer>({this->_graphics_command_buffer}));
	

}

void vkEngine::cleanup()
{

	this->_device.destroyCommandPool(this->_command_pool);
#ifndef NDEBUG
	this->_instance.destroyDebugUtilsMessengerEXT(this->_debug_messanger);
#endif // !NDEBUG
	this->_device.destroy();
	this->_instance.destroy();
}

vkEngine::vkEngine(SDL_Window* window):
	_window(window)
{
}

void vkEngine::_init_command_pool(int queue_family_index)
{
	vk::CommandPoolCreateInfo create_command_pool_info{
		.flags = { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },
		.queueFamilyIndex = static_cast<uint32_t>(queue_family_index)
	};
	this->_command_pool = this->_device.createCommandPool(create_command_pool_info);
}

void vkEngine::_allocate_command_buffer(std::vector<vk::CommandBuffer> buffer_list)
{
	vk::CommandBufferAllocateInfo buffer_allocation_info{
		.commandPool = this->_command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = static_cast<uint32_t>(buffer_list.size())
	};
	buffer_list = this->_device.allocateCommandBuffers(buffer_allocation_info);
}

void vkEngine::_record_command_buffer()
{
}



