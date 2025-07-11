#version 330 core

in vec2 vTexCoord;

uniform sampler2D u_Texture;
uniform int u_Time;
uniform bool u_FlipX;

out vec4 FragColor;

void main() {
    vec2 uv = vTexCoord;
    if (u_FlipX) {
        uv.x = 1.0 - uv.x;
    }
    FragColor = texture(u_Texture, uv);
}
