#version 330 core

in vec2 vTexCoord;
uniform vec4 u_Color;
out vec4 FragColor;

void main() {
    FragColor = u_Color;
}
