#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D u_Texture;

void main() {
    vec4 texColor = texture(u_Texture, vTexCoord);

    if (texColor.a < 0.1) discard;

    FragColor = texColor;
}
