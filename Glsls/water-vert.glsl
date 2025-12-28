#version 410

layout (location = 0) in vec3 position;

out vec3 pass_world_position;

uniform mat4 uni_M;
uniform mat4 uni_VP;

void main()
{
	vec4 world_position = uni_M * vec4(position, 1.0);
	pass_world_position = world_position.xyz;
	gl_Position = uni_VP * world_position;
}