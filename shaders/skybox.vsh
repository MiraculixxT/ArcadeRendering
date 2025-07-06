#version 410 core
layout (location = 0) in vec3 aPos;
out vec3 texCoords;

uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    texCoords = aPos;
    vec4 pos = uProjection * mat4(mat3(uView)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}