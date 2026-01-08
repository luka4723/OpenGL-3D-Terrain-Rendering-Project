#version 330 core
out vec4 FragColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform vec3 color;
void main()
{
    FragColor = vec4(color,1.0);
}
