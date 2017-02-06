#version 410

layout(location = 0) in vec3 f_color;

layout(location = 0) out vec4 out_color;

void main(void) {
    out_color = vec4(f_color, 1.0);
}
