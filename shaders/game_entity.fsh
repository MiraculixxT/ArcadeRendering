#version 330 core
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D u_Texture;

void main() {
    fragColor = texture(u_Texture, texCoord);
}
