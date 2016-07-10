#version 330

in vec3 fPosCamera;
in vec3 fNrmCamera;

out vec4 outColor;

uniform samplerCube uCubemap;

void main(void) {
    vec3 V = normalize(-fPosCamera);
    vec3 N = normalize(fNrmCamera);
    vec3 R = V - 2.0 * (V - N * dot(N, V));

    outColor = vec4(texture(uCubemap, R).rgb, 1.0);
}
