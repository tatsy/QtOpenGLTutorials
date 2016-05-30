#version 430

// Input
in vec3 vPosition;
in vec3 vNormal;
in vec4 vColor;

// Output
out vec3 fPosition;
out vec3 fNormal;
out vec4 fColor;
out vec3 fLightPos;

// Uniform
uniform mat4 uMVPMat;
uniform mat4 uMVMat;
uniform vec3 uLightPos;

// Main
void main(void) {
    gl_Position = uMVPMat * vec4(vPosition, 1.0);
    fPosition = (uMVMat * vec4(vPosition, 1.0)).xyz;
    fNormal   = (transpose(inverse(uMVMat)) * vec4(vNormal, 1.0)).xyz;
    fColor    = vColor;
    fLightPos = (uMVMat * vec4(uLightPos, 1.0)).xyz;
}
