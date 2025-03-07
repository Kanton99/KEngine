#include "vkEngine.hpp"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct VectorData {
  glm::vec3 position;
  uint32_t uv_x;
  glm::vec3 normals;
  uint32_t uv_y;
  glm::vec4 colour;
};

struct GPUBufferDate {
  mvk::AllocatedBuffer vectorBuffer;
  mvk::AllocatedBuffer indexBuffer;
  vk::DeviceMemory vectorBufferAddress;
};
