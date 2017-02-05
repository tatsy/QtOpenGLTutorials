#version 410

layout(location = 0) in vec3 posWorldSpace;
layout(location = 2) in vec2 texCoord;

out vec4 outColor;

uniform mat4 u_viewMat;
uniform mat4 u_modelMat;
uniform vec3 u_lightPos;
uniform sampler2D u_texture;

void main(void) {
    mat4 mvMat = u_modelMat * u_viewMat;
    mat4 normMat = inverse(transpose(mvMat));
    vec3 normWorldSpace = normalize(cross(dFdx(posWorldSpace), dFdy(posWorldSpace)));

    vec3 posViewSpace = (mvMat * vec4(posWorldSpace, 1.0)).xyz;
    vec3 normViewSpace = (normMat * vec4(normWorldSpace, 0.0)).xyz;
    vec3 lightPosViewSpace = (mvMat * vec4(u_lightPos, 1.0)).xyz;

    vec3 V = normalize(-posViewSpace);
    vec3 N = normalize(normViewSpace);
    vec3 L = normalize(lightPosViewSpace - posViewSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffColor = texture(u_texture, texCoord).rgb;
    vec3 specColor = vec3(0.5, 0.5, 0.5);

    outColor.rgb = ndotl * diffColor + pow(ndoth, 128.0) * specColor;
    outColor.a = 1.0;
}
