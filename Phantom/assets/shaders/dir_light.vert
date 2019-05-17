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
layout(location = 1) out vec3 m_position;
layout(location = 3) out vec3 norm;

void main() {
    gl_Position = mvp.proj * (mvp.view * (mvp.model * vec4(inPosition, 1.0)));
    TextCoord = inTextCoord_0;
    m_position = vec3(mvp.model * vec4(inPosition, 1.0));
    norm = vec3(mvp.model * vec4(normalize(inNormal), 0.0));
}