#version 330 core

layout (location = 4) in vec2 VertexTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;

    gl_Position = vec4(VertexTexCoord * 2.0 - 1.0, 0.0, 1.0);

}
