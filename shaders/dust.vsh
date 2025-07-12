#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uSize;

out vec2 vTex;

void main() {
    vec2 offsets[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  1.0)
    );

    vec2 offset = offsets[gl_VertexID % 6];
    vec3 worldPos = aPos + uCameraRight * offset.x * uSize + uCameraUp * offset.y * uSize;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
    vTex = offset * 0.5 + 0.5;
}