#include "vkEngine.h"
#include <iostream>
#include <SDL_vulkan.h>

template <typename T>
void vkb_error_hanlder(T ret) {
	if (!ret) {
		std::cerr << "Failed to build " << typeid(ret).name() << ", Error: " << ret.error().message() << std::endl;
		exit(-1);
	}
}

vkEngine::vkEngine(SDL_Window* window) :
	_window(window)
{
}

vkEngine* vkEngine::get(SDL_Window* window)
{
	if (!_engine) {
		_engine = new vkEngine(window);
	}
	return nullptr;
}

void vkEngine::init()
{
#pragma region Instance init
	vkb::InstanceBuilder builder;
	auto inst_ret = builder.set_app_name("Vulkan Engine for KEngine")
		.set_engine_name("vkEngine")
#ifndef NDEBUG
		.request_validation_layers().use_default_debug_messenger()
#endif // DEBUG
		.require_api_version(1, 3, 0).build();
	/*if (!inst_ret) {
		std::cerr << "Failed to build instance, Error: " << inst_ret.error().message() << std::endl;
		exit(-1);
	}*/
	vkb_error_hanlder(inst_ret);
	auto vkb_instance = inst_ret.value();
	this->_instance = vkb_instance.instance;
	this->_debug_messanger = vkb_instance.debug_messenger;

	auto err = SDL_Vulkan_CreateSurface(this->_window, this->_instance, &(_surface));
	if (!err) {
		std::cerr << "Failed to create surface, Error: " << SDL_GetError() << std::endl;
		exit(-1);
	}
#pragma endregion

#pragma region Device Init
	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	vkb::PhysicalDeviceSelector phy_dev_sel(vkb_instance);
	auto phys_dev_sel_ret = phy_dev_sel.set_surface(this->_surface)
		.set_required_features_13(features)
		.select();
	vkb_error_hanlder(phys_dev_sel_ret);
	auto phys_device = phys_dev_sel_ret.value();
	this->_physDevice = phys_device.physical_device;

	vkb::DeviceBuilder device_builder{ phys_device };
	auto dev_ret = device_builder.build();
	vkb_error_hanlder(dev_ret);

	auto vkb_device = dev_ret.value();
	this->_device = vkb_device.device;
#pragma endregion

#pragma region Queue Init
	auto graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics);
	vkb_error_hanlder(graphics_queue);

	this->_graohics_queue = graphics_queue.value();
#pragma endregion

	//vkb::destroy_instance(vkb_instance);
	std::cout << "------------------------\n" <<  "Engine started correctly\n" << "------------------------\n" << std::endl;
}

void vkEngine::cleanup()
{
	vkDestroyDevice(this->_device, nullptr);

	vkDestroySurfaceKHR(this->_instance, this->_surface, nullptr);

	vkb::destroy_debug_utils_messenger(this->_instance, this->_debug_messanger, nullptr);

	vkDestroyInstance(this->_instance, nullptr);
	std::cout << "Engine cleaned correctly" << std::endl;
}

