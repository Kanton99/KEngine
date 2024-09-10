#include "vkEngine.h"


#ifndef DYNAMIC_STORAGE_CREATED
#define DYNAMIC_STORAGE_CREATED
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
	VULKAN_HPP_DEFAULT_DISPATCHER.init();

	vk::ApplicationInfo app_info("KEngine",1,"vkEngine",VK_API_VERSION_1_3);
	vk::InstanceCreateInfo instance_info({}, &app_info);
	this->_instance = vk::createInstance(instance_info);

#ifndef NDEBUG
	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo{
		{},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		nullptr
	};
	this->_debug_messanger = this->_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerInfo);
#endif // !DEBUG

}

void vkEngine::cleanup()
{

#ifndef NDEBUG
	this->_instance.destroyDebugUtilsMessengerEXT(this->_debug_messanger);
#endif // !NDEBUG

	this->_instance.destroy();
}

vkEngine::vkEngine(SDL_Window* window):
	_window(window),
	_dynamicStorageCreated(false)
{
}
