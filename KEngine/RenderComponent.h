#pragma once
#include "VulkanEngine.h"
struct RenderComponent
{
	VkBuffer *vertices;
	VkBuffer *indices;

	VkDescriptorSet* descriptor;
};