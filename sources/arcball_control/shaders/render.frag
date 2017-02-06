#version 410

layout(location = 0) in vec3 f_posViewSpace;
layout(location = 1) in vec3 f_normViewSpace;
layout(location = 2) in vec3 f_lightPosViewSpace;

layout(location = 0) out vec4 out_color;

void main(void) {
    vec3 V = normalize(-f_posViewSpace);
    vec3 N = normalize(f_normViewSpace);
    vec3 L = normalize(f_lightPosViewSpace - f_posViewSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    float brightness = 0.5 * ndotl + 0.5 * pow(ndoth, 16.0);
    out_color = vec4(brightness, brightness, brightness, 1);
}
