#version 450

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(location = 0) out vec3 fragColor;

vec3 colors[4] = vec3[](
    vec3(1.0, 0.0, 0.0),  //RED
    vec3(0.0, 1.0, 0.0),  //GREEN
    vec3(0.0, 0.0, 1.0),  //BLUE
    vec3(1.0, 0.0, 1.0)   //YELLOW
);

layout(location = 0) in vec3 inPosition;
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = colors[gl_VertexIndex];
}
