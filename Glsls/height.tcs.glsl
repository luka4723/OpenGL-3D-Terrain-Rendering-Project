#version 410 core

layout(vertices=4) out;

uniform mat4 model;
uniform vec3 cameraPos;

in vec2 TexCoord[];
out vec2 TextureCoord[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    if(gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 4;
        const int MAX_TESS_LEVEL = 64;
        const float MIN_DISTANCE = 1;
        const float MAX_DISTANCE = 130;

        vec4 worldPos00 =  model * gl_in[0].gl_Position;
        vec4 worldPos01 =  model * gl_in[1].gl_Position;
        vec4 worldPos10 =  model * gl_in[2].gl_Position;
        vec4 worldPos11 =  model * gl_in[3].gl_Position;

        float dist00 = distance(worldPos00.xyz, cameraPos);
        float dist01 = distance(worldPos01.xyz, cameraPos);
        float dist10 = distance(worldPos10.xyz, cameraPos);
        float dist11 = distance(worldPos11.xyz, cameraPos);

        float distance00 = clamp( (dist00 - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
        float distance01 = clamp( (dist01 - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
        float distance10 = clamp( (dist10 - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
        float distance11 = clamp( (dist11 - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );

        float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
        float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
        float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
        float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}