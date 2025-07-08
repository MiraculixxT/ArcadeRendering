#version 330 core

in vec2 vTexCoord;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main() {
    FragColor = texture(u_Texture, vTexCoord);
}
