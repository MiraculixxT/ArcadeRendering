#version 330 core

in vec2 vTexCoord;
in vec2 vScreenUV;

uniform sampler2D u_Texture;
uniform float u_Time;
uniform bool u_Static = false;

out vec4 FragColor;

const vec2 screenSize = vec2(1920.0, 1080.0);

uniform float colorLevels = 6.0;
uniform float noiseStrength = 0.05;      // grain intensity
uniform float noiseScale = 3.47;         // pixel size for noise
uniform float scanlineStrength = 0.255;  // 0.0 = none, 1.0 = black lines
uniform float scanlineFrequency = 0.25;  // 1.0 = every line, 0.5 = every 2nd line

const float leftEdge = 0.21875;
const float rightEdge = 0.78125;


vec3 applyRetroColors(vec3 color) {
    // Shift colors to more retro tones (teal/purple)
    color.r = pow(color.r, 0.95);
    color.g = pow(color.g, 1.05);
    color.b = pow(color.b, 1.1);

    // Color quantization for banding
    color = floor(color * colorLevels) / colorLevels;

    return color;
}

float vignette() {
    float dist = distance(vScreenUV, vec2(0.5));
    return smoothstep(0.8, 0.3, dist) * 2.0f;
}

float scanline() {
    float lines = screenSize.y;
    float y = vScreenUV.y * lines * scanlineFrequency;
    return 1.0 - scanlineStrength * step(0.5, fract(y));
}

float noise(float t) {
    vec2 grainUV = floor(vScreenUV * screenSize / noiseScale);
    float n = fract(sin(dot(grainUV + t, vec2(12.9898, 78.233))) * 43758.5453);
    return n;
}

float edgeFade() {
    float edgeSoftness = 10.0 / screenSize.x;  // convert pixels to UV

    float leftBlend = smoothstep(leftEdge, leftEdge + edgeSoftness, vScreenUV.x);
    float rightBlend = smoothstep(rightEdge, rightEdge - edgeSoftness, vScreenUV.x);

    return min(leftBlend, rightBlend);  // 1.0 inside, fades to 0.0 at edges
}


void main() {
    vec4 color = texture(u_Texture, vTexCoord);

    if (!u_Static) color.rgb = applyRetroColors(color.rgb);

    if (vScreenUV.x > leftEdge && vScreenUV.x < rightEdge) {
        float fade = edgeFade();
        vec3 bgColor = vec3(0x05 / 255.0); // background color
        color.rgb = mix(bgColor, color.rgb, fade);  // fade into active color
    }

    color.rgb *= vignette();
    color.rgb *= scanline();

    if (!u_Static) {
        float grain = noise(u_Time);
        color.rgb += (grain - 0.5) * noiseStrength;
    }

    FragColor = color;
}
