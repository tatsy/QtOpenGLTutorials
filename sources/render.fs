#version 430

in vec3 fPosition;
in vec3 fNormal;
in vec4 fColor;
in vec3 fLightPos;

out vec4 outColor;

void main(void) {
    vec3 V = normalize(-fPosition);
    vec3 N = normalize(fNormal);
    vec3 L = normalize(fLightPos - fPosition);
    float diffuse = max(0.0, dot(N, L));
    outColor = vec4(fColor.rgb * diffuse, fColor.a);
}
