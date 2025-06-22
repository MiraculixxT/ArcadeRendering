#version 330 core

// Values from vertex shader
in vec3 interpNormal;
in vec3 worldPosition;

// Outgoing values (mostly color)
out vec3 fragColor;

// Uniforms
//uniform <type> <name>;

void main() {
    fragColor = vec3(0.0); // blackscreen
}
