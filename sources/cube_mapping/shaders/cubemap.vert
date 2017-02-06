#version 410

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec4 f_texcoord;

uniform mat4 u_mvpMat;

void main(void) {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    f_texcoord = vec4(in_position, 1.0);
}
