#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vulkan.h>
#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
//
//const std::vector<Vertex> vertices = {
//	{{-0.5f, -0.5f,0.f}, {1.0f, 0.0f, 0.0f}, {1.f,0.f}},
//	{{0.5f, -0.5f,0.f}, {0.0f, 1.0f, 0.0f},{0.f,0.f}},
//	{{0.5f, 0.5f,0.f}, {0.0f, 0.0f, 1.0f},{0.f,1.f}},
//	{{-0.5f,0.5f,0.f},{1.0f,1.f,1.f},{1.f,1.f}},
//
//	{{-0.5f, -0.5f,-0.5f}, {1.0f, 0.0f, 0.0f}, {1.f,0.f}},
//	{{0.5f, -0.5f,-0.5f}, {0.0f, 1.0f, 0.0f},{0.f,0.f}},
//	{{0.5f, 0.5f,-0.5f}, {0.0f, 0.0f, 1.0f},{0.f,1.f}},
//	{{-0.5f,0.5f,-0.5f},{1.0f,1.f,1.f},{1.f,1.f}}
//};
//
//const std::vector<uint16_t> indeces = {
//	0,1,2,2,3,0,
//	4,5,6,6,7,4
//};