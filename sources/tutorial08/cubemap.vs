#version 330

layout(location = 0) in vec3 vPosition;

out vec4 fTexCoord;

uniform mat4 uMVPMat;

void main(void) {
    gl_Position = (uMVPMat * vec4(vPosition, 1.0));
    fTexCoord = vec4(vPosition, 1.0);
}
