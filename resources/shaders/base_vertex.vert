#version 450

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(location = 0) out vec3 fragColor;

vec3 colors[8] = vec3[](
    vec3(1.0, 1.0, 0.0),  //YELLOW
    vec3(1.0, 0.0, 0.0),  //RED
    vec3(1.0, 1.0, 1.0),   //WHITE
    vec3(1.0, 0.0, 1.0),  //MAGENTA
    vec3(0.0, 1.0, 0.0),  //GREEN
    vec3(0.0, 0.0, 0.0),  //BLACK
    vec3(0.0, 1.0, 1.0),  //CYAN
    vec3(0.0, 0.0, 1.0)  //BLUE
);

layout(location = 0) in vec3 inPosition;
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = colors[gl_VertexIndex];
}
