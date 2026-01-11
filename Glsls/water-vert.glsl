#version 410 core

layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 uni_M;
uniform mat4 uni_VP;

void main()
{
	FragPos = vec3(uni_M * vec4(aPos, 1.0));
	gl_Position = uni_VP * vec4(FragPos,1.0);
}