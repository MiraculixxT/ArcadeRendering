#version 330 core

in vec3 fragNormal;
in vec3 worldPosition;

out vec4 fragColor;

uniform vec3 uCameraPos;
uniform vec3 uLightDir = normalize(vec3(1.0, 1.0, 1.0));
uniform vec3 uAlbedo = vec3(0.8, 0.2, 0.2);

void main() {
    vec3 N = normalize(fragNormal);                      // Korrektur hier
    vec3 L = normalize(-uLightDir);
    vec3 V = normalize(uCameraPos - worldPosition);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * uAlbedo;

    fragColor = vec4(diffuse, 1.0);
}