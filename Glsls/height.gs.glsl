#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 9) out;

in vec3 Normal2[];
in float Height[];

const float MAGNITUDE = 0.4;

out float HeightOut;

uniform mat4 projection;

void GenerateLine(int index)
{

    gl_Position =  gl_in[index].gl_Position;
    HeightOut = Height[index];
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position + vec4(Normal2[index], 0.0) * MAGNITUDE);
    HeightOut = Height[index];
    EmitVertex();
    EndPrimitive();
}

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        HeightOut = Height[i];
        EmitVertex();
    }
    EndPrimitive();

    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}