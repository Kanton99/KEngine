#include "vkEngine.h"
#include <iostream>

void vkEngine::init()
{
	vkb::InstanceBuilder builder;
	auto inst_ret = builder.set_app_name("Vulkan Engine for KEngine")
#ifndef NDEBUG
		.request_validation_layers().use_default_debug_messenger()
#endif // DEBUG
		.require_api_version(1,3,0).build();
	if (!inst_ret) {
		std::cerr << "Failed to build instance, Error: "<< inst_ret.error().message() << std::endl;
		exit(-1);
	}

	this->_instance = inst_ret.value().instance;



	std::cout << "Engine started correctly" << std::endl;
}
