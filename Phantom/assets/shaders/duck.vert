#version 450

layout(binding = 0) uniform MeshUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec2 inTextCoord_0;

layout(location = 0) out vec2 TextCoord;

void main() {
    gl_Position = mvp.proj * (mvp.view * (mvp.model * vec4(inPosition, 1.0)));
    TextCoord = inTextCoord_0;
}