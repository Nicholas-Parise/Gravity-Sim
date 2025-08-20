#version 120
/*
layout(location=0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoords;

out vec2 fragUV;
out vec4 fragColor;
*/
uniform mat4 projection;

void main()
{
    //fragUV = texCoords;
    //fragColor = color;
    //gl_Position = projection * vec4(position, 0.0, 1.0);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // forward the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // forward the vertex color
    gl_FrontColor = gl_Color;
}

