#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 tescPosition;
layout(location = 1) out vec3 tescNormal;
layout(location = 2) out vec2 tescTexCoord;

void main(void) {
    tescPosition = inPosition;
    tescNormal = inNormal;
    tescTexCoord = inTexCoord;
}
