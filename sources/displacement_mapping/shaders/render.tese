#version 410

layout(triangles, equal_spacing, ccw) in;

layout(location = 0) in vec3 tesePosition[];
layout(location = 1) in vec3 teseNormal[];
layout(location = 2) in vec2 teseTexCoord[];

layout(location = 0) out vec3 geoPosition;
layout(location = 2) out vec2 geoTexCoord;

uniform sampler2D u_texture;

void main(void) {
    vec3 p0 = gl_TessCoord.x * tesePosition[0];
    vec3 p1 = gl_TessCoord.y * tesePosition[1];
    vec3 p2 = gl_TessCoord.z * tesePosition[2];
    vec3 position = p0 + p1 + p2;

    vec3 n0 = gl_TessCoord.x * teseNormal[0];
    vec3 n1 = gl_TessCoord.y * teseNormal[1];
    vec3 n2 = gl_TessCoord.z * teseNormal[2];
    vec3 normal = n0 + n1 + n2;

    vec2 tc0 = gl_TessCoord.x * teseTexCoord[0];
    vec2 tc1 = gl_TessCoord.y * teseTexCoord[1];
    vec2 tc2 = gl_TessCoord.z * teseTexCoord[2];
    geoTexCoord = tc0 + tc1 + tc2;

    vec3 rgb = texture(u_texture, geoTexCoord).xyz;
    float height = (0.30 * rgb.r + 0.59 * rgb.g + 0.11 * rgb.b);
    position += normal * height * 0.05;
    geoPosition = position;
}
