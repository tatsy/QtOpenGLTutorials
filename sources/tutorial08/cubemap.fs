#version 330

in vec4 fTexCoord;

out vec4 outColor;

uniform samplerCube uCubemap;

void main(void) {
    outColor = texture(uCubemap, fTexCoord.xyz);
}
