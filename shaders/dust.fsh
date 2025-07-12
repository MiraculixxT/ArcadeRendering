#version 330 core
in vec2 vTex;
out vec4 FragColor;

uniform sampler2D uTex;

uniform float uAlpha;

uniform vec3 uLightPos[4];
uniform vec3 uLightColor[4];
uniform int uLightCount;
uniform vec3 uFragPos;

void main() {
    vec4 texColor = texture(uTex, vTex);
    if (texColor.a < 0.1)
        discard;

    vec3 lightAccum = vec3(0.0);
    for (int i = 0; i < uLightCount; ++i) {
        vec3 lightVec = uLightPos[i] - uFragPos;
        float dist = length(lightVec);
        if (dist < 15.0) {
            float attenuation = smoothstep(15.0, 3.75, dist);
            lightAccum += uLightColor[i] * attenuation;
        }
    }

    vec3 ambient = 0.1 * texColor.rgb;
    vec3 finalColor = ambient + texColor.rgb * lightAccum;
    FragColor = vec4(finalColor, texColor.a * uAlpha);
}