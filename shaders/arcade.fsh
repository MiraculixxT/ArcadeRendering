#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragViewDir;
in vec3 fragWorldPos;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
const int MAX_POINT_LIGHTS = 8;
uniform int uNumPointLights;
uniform struct {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
} uPointLights[MAX_POINT_LIGHTS];

uniform sampler2D tex0;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragViewDir);
    vec3 lightDir = normalize(uLightDirection);

    vec4 texColor = texture(tex0, fragTexCoord);
    if (texColor.a < 0.1)
        discard;

    // Ambient
    vec3 ambient = uAmbientColor * texColor.rgb;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * texColor.rgb;

    // Point Lights
    vec3 pointLightResult = vec3(0.0);
    for (int i = 0; i < uNumPointLights; ++i) {
        vec3 lightVec = uPointLights[i].position - fragWorldPos;
        float distance = length(lightVec);
        if (distance < uPointLights[i].radius) {
            vec3 lightDirPoint = normalize(lightVec);
            float attenuation = smoothstep(uPointLights[i].radius, uPointLights[i].radius * 0.25, distance);
            float NdotL = dot(normal, lightDirPoint);
            float diffPoint = smoothstep(0.0, 0.3, NdotL);
            pointLightResult += texColor.rgb * uPointLights[i].color * diffPoint * uPointLights[i].intensity * attenuation;
        }
    }

    fragColor = vec4(ambient + diffuse + pointLightResult, texColor.a);
}