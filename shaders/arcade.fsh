#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragViewDir;
in vec3 fragWorldPos;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;

uniform sampler2D uShadowMap;
uniform mat4 uLightSpaceMatrix;

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
    // Normalize inputs
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

    // Shadow Mapping
    vec4 fragPosLight = uLightSpaceMatrix * vec4(fragWorldPos, 1.0);
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.00001 * (1.0 - dot(normal, lightDir)), 0.00025);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 0.4 : 1.0;
        }
    }
    shadow /= 9.0;

    fragColor = vec4((ambient + (diffuse + pointLightResult) * shadow), texColor.a);}