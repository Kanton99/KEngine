#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "util_structs.hpp"

namespace mvk{
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
    vk::DeviceAddress vectorBufferAddress;
  };

  struct GPUDrawPushConstants {
    glm::mat4 modelMatrix;
    vk::DeviceAddress vertexBuffer;
  };
}
#endif
