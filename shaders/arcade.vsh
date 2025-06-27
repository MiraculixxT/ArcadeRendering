#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 uWorldToClip;
uniform mat4 uModelMatrix;
uniform vec3 uCameraPos;

out vec3 fragNormal;
out vec3 viewDir;
out vec3 worldPosition;

void main() {
    mat4 uLocalToWorld = uModelMatrix;
    mat3 uNormalMatrix = transpose(inverse(mat3(uModelMatrix)));

    gl_Position = uWorldToClip * uModelMatrix * vec4(position, 1.0);
    vec3 worldPos = (uModelMatrix * vec4(position, 1.0)).xyz;
    fragNormal = normalize(uNormalMatrix * normal);
    viewDir = normalize(uCameraPos - worldPos);
    worldPosition = worldPos;
}