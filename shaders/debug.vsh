#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

uniform mat4 uLocalToClip;
uniform mat4 uLocalToWorld;

out vec3 interpNormal;
out vec3 worldPosition;

void main() {
    gl_Position = uLocalToClip * vec4(position, 1.0);
    worldPosition = (uLocalToWorld * vec4(position, 1.0)).xyz;
    interpNormal = (uLocalToWorld * vec4(normal, 0.0)).xyz;
}
