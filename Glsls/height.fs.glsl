#version 410 core

in float Height;

out vec4 FragColor;

uniform sampler2D gradient;

void main()
{
    float h = (Height + 20)/128.0f;
    vec3 col = texture(gradient,vec2(h,0.5)).rgb;
    FragColor = vec4(col*0.2f, 1.0);
}