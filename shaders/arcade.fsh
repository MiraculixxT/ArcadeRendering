#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragViewDir;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;

uniform sampler2D tex0;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragViewDir);
    vec3 lightDir = normalize(uLightDirection);

    vec3 texColor = texture(tex0, fragTexCoord).rgb;

    // Ambient
    vec3 ambient = uAmbientColor * texColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * texColor;

    fragColor = vec4(ambient + diffuse, 1.0);
}