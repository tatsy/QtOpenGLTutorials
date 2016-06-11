#version 330

in vec3 fPosCamera;
in vec3 fNrmCamera;
in vec3 fLightPosCamera;

out vec4 outColor;

uniform vec3  uDiffuse;
uniform vec3  uSpecular;
uniform vec3  uAmbient;
uniform float uShininess;

void main(void) {
    vec3 V = normalize(-fPosCamera);
    vec3 N = normalize(fNrmCamera);
    vec3 L = normalize(fLightPosCamera - fPosCamera);
    vec3 H = normalize(V + L);

    float NdotL = max(0.0, dot(N, L));
    float NdotH = max(0.0, dot(N, H));

    vec3 diffRgb = uDiffuse.rgb * NdotL;
    vec3 specRgb = uSpecular.rgb * pow(NdotH, uShininess);
    vec3 ambiRgb = uAmbient.rgb;
    vec3 rgb = diffRgb + specRgb + ambiRgb;

    outColor = vec4(rgb, 1.0);
}
