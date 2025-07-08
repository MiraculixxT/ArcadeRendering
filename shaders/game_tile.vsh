#version 330 core

layout (location = 0) in vec2 aPos;       // vertex position (e.g., quad corner)
layout (location = 1) in vec2 aTexCoord;  // UV coordinate (0-1 range)

uniform mat4 u_MVP;  // full MVP matrix from C++

out vec2 vTexCoord;

void main() {
    gl_Position = u_MVP * vec4(aPos, 0.0, 1.0); // 2D quad in 3D space
    vTexCoord = aTexCoord;
}
