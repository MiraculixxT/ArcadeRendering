#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 1) in vec2 texCoord;

uniform mat4 uModelMatrix;
uniform mat4 uWorldToClip;
uniform vec3 uCameraPos;

out vec3 fragNormal;
out vec3 fragViewDir;
out vec2 fragTexCoord;
out vec3 fragWorldPos;

void main() {
    vec4 worldPos = uModelMatrix * vec4(position, 1.0);
    fragWorldPos = worldPos.xyz;
    fragNormal = mat3(transpose(inverse(uModelMatrix))) * normal;
    fragViewDir = normalize(uCameraPos - worldPos.xyz);
    fragTexCoord = texCoord;

    gl_Position = uWorldToClip * worldPos;
}