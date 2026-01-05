#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
    if(FragColor.a < 0.1) // bolje radi on glEnable(GL_BLEND) jer bih onda morao da crtam prvo najdalji providni objekat
    discard;
}