#version 410

layout(location = 0) in vec3 f_posViewSpace;
layout(location = 1) in vec3 f_normViewSpace;
layout(location = 2) in vec3 f_lightPosViewSpace;
layout(location = 3) in vec2 f_texcoord;

layout(location = 0) out vec4 out_color;

uniform sampler2D u_texture;

void main(void) {
    vec3 V = normalize(-f_posViewSpace);
    vec3 N = normalize(f_normViewSpace);
    vec3 L = normalize(f_lightPosViewSpace - f_posViewSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffColor = texture(u_texture, f_texcoord).rgb;
    vec3 specColor = vec3(0.5, 0.5, 0.5);
    out_color.rgb = diffColor * ndotl + specColor * pow(ndoth, 16.0);
    out_color.a = 1.0;
}
