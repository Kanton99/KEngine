#version 450
#extension GL_EXT_buffer_reference : require

struct Vertex {
  vec3 position;
  float uv_x;
  vec3 normal;
  float uv_y;
  vec4 color;
}

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(buffer_referece, std430) readonly buffer VertexBuffer {
  Vertex vertices[];
};

layout(push_constant) uniform constants{
  mat4 model_matrix;
  VertexBuffer vertexBuffer;
} PushConstants;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outUV;

void main() {
  Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
  fragColor = v.color.xyz;
  outUV.x = v.uv_x;
  outUV.y = v.uv_y;
}
