#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 u_mvpMat;

void main(void) {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
}
