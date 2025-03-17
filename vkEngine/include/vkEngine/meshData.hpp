#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "util_structs.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace mvk{
  struct VertexData {
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 colour = glm::vec4(1,1,1,1);

    bool operator==(const VertexData& other) const {
      return position==other.position && uv_x == other.uv_x && normal==other.normal && uv_y==other.uv_y && colour==other.colour;
    }
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

namespace std{
template<> struct hash<mvk::VertexData>{
  size_t operator()(mvk::VertexData const& vertex) const {
    return (
      (hash<glm::vec3>()(vertex.position) ^ 
      hash<glm::vec3>()(vertex.normal) ^
      hash<glm::vec3>()(vertex.colour) ^
      hash<float>()(vertex.uv_x) ^ hash<float>()(vertex.uv_y)
  )
    );
  }
};
}
#endif
