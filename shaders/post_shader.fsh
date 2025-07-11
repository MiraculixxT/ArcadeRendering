#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_ScreenTexture;
uniform float u_Time;
uniform int u_State;

void main() {
    // Only render red for debugging
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    return;

    vec3 color = texture(u_ScreenTexture, TexCoord).rgb;

    switch (u_State) {
        case 0: // Dark
            break;

        case 1: // Light
            break;

        case 2: // Game
            // retro palette tint
            color.r *= 1.1;
            color.g *= 0.95;
            color.b *= 0.9;

            // Add scanlines
            float scanline = sin(TexCoord.y * 1080.0) * 0.05; // maybe pass screen height as uniform, but resolution is kinda fixed atm
            color -= scanline;
            break;
    }

    FragColor = vec4(color, 1.0);
}
