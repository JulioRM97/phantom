#version 450

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

layout(binding = 0) uniform MeshUniform {
    vec4 color;
} ubo;

layout(binding = 1) uniform Mio {
    vec4 color;
} mio;


layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = ubo.color.rgb + mio.color.rgb;
}