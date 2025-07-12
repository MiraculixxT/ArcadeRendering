#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// Uniforms
uniform mat4 u_MVP;

// Output to fragment shader
out vec2 vTexCoord;
out vec2 vScreenUV;     // normalized screen-space UV (0.0–1.0)

void main() {
    vec4 clipPos = u_MVP * vec4(aPosition, 1.0);
    gl_Position = clipPos;

    // Convert from clip space (-1 to 1) to screen UV (0 to 1)
    vScreenUV = clipPos.xy / clipPos.w;      // NDC
    vScreenUV = vScreenUV * 0.5 + 0.5;        // to 0–1 range

    vTexCoord = aTexCoord;
}
