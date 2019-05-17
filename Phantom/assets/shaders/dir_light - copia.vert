#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform LightData {
    vec3 position;
    float range;
    vec3 camera_pos;
    float intensity;
	  vec3 direction;
} light_data;

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec2 TextCoord;
layout(location = 1) in vec3 m_position;
layout(location = 3) in vec3 norm;

layout(location = 0) out vec4 outColor;

void main() 
{
  vec3 normal = normalize(norm);
  vec3 lightDir = normalize(-light_data.direction);
	vec3 view =  normalize(light_data.camera_pos - m_position);

  vec3 reflectDir = reflect(lightDir, normal);
	
	float spec = pow(max(dot(view, -reflectDir), 0.0), 32.0);
	float diff = max(dot(norm, lightDir), 0.0);


  outColor = texture(texSampler, TextCoord) * (spec + diff) * light_data.intensity;
}