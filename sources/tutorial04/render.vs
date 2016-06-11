#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec4 fColor;

uniform mat4 uMVPMat;

void main(void) {
    gl_Position = uMVPMat * vec4(vPosition, 1.0);
}
