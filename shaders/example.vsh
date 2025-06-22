#version 330 core

// Vertex shader for rendering a 3D object with normals and world position
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

// Uniforms
uniform mat4 uLocalToClip;
uniform mat4 uLocalToWorld;

// Values passed to the fragment shader
out vec3 interpNormal;
out vec3 worldPosition;

void main() {
    gl_Position = uLocalToClip * vec4(position, 1.0);
    worldPosition = (uLocalToWorld * vec4(position, 1.0)).xyz;
    interpNormal = (uLocalToWorld * vec4(normal, 0.0)).xyz;
}