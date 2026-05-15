#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 4) in vec2 VertexTexCoord;

out vec3 WorldPos;

uniform mat4 WorldMatrix;

void main()
{
    WorldPos = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;

    gl_Position = vec4(VertexTexCoord * 2.0 - 1.0, 0.0, 1.0);

}