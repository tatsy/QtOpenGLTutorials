#version 410

layout(vertices = 3) out;

layout(location = 0) in vec3 tescPosition[];
layout(location = 1) in vec3 tescNormal[];
layout(location = 2) in vec2 tescTexCoord[];

layout(location = 0) out vec3 tesePosition[];
layout(location = 1) out vec3 teseNormal[];
layout(location = 2) out vec2 teseTexCoord[];

void main(void) {
    tesePosition[gl_InvocationID] = tescPosition[gl_InvocationID];
    teseNormal[gl_InvocationID] = tescNormal[gl_InvocationID];
    teseTexCoord[gl_InvocationID] = tescTexCoord[gl_InvocationID];

    gl_TessLevelInner[0] = 20.0;
    gl_TessLevelOuter[0] = 10.0;
    gl_TessLevelOuter[1] = 10.0;
    gl_TessLevelOuter[2] = 10.0;
}
