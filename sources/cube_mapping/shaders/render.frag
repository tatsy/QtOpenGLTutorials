#version 410

layout(location = 0) in vec3 f_posViewSpace;
layout(location = 1) in vec3 f_normViewSpace;

layout(location = 0) out vec4 out_color;

uniform samplerCube u_cubemap;

void main(void) {
    vec3 V = normalize(-f_posViewSpace);
    vec3 N = normalize(f_normViewSpace);
    vec3 R = V - 2.0 * (V - N * dot(N, V));

    out_color.rgb = texture(u_cubemap, R).rgb;
    out_color.a = 1.0;
}
