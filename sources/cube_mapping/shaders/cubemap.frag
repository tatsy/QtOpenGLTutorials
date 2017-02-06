#version 410

layout(location = 0) in vec4 f_texcoord;

layout(location = 0) out vec4 out_color;

uniform samplerCube u_cubemap;

void main(void) {
    out_color = texture(u_cubemap, f_texcoord.xyz);
}
