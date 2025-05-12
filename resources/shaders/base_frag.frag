#version 460
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set=0, binding=1) uniform sampler2D displayTexture;

void main() {
    // outColor = vec4(fragColor, 1.0);
  outColor = texture(displayTexture, inUV);
}
