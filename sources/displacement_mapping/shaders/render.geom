#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

layout(location = 0) in vec3 geoPosition[];
layout(location = 2) in vec2 geoTexCoord[];

layout(location = 0) out vec3 posWorldSpace;
layout(location = 2) out vec2 texCoord;

uniform mat4 u_projMat;
uniform mat4 u_viewMat;
uniform mat4 u_modelMat;

void main(void) {
    mat4 mvpMat = u_projMat * u_viewMat * u_modelMat;

    for (int i = 0; i < 3; i++) {
        gl_Position = mvpMat * vec4(geoPosition[i], 1.0);
        posWorldSpace = geoPosition[i];
        texCoord = geoTexCoord[i];
        EmitVertex();
    }
    EndPrimitive();
}
