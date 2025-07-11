#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// Uniforms
uniform mat4 u_MVP;

// Output to fragment shader
out vec2 vTexCoord;

void main() {
    gl_Position = u_MVP * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
}
