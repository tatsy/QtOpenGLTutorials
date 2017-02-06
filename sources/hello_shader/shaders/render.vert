#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 f_color;

void main(void) {
    gl_Position = vec4(in_position, 1.0);
    f_color = in_color;
}
