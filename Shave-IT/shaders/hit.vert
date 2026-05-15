#version 330

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Normal;

uniform mat4 WorldMatrix;
uniform mat4 WorldViewProjMatrix;

void main()
{
    Normal = normalize(mat3(WorldMatrix) * VertexNormal);
    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
}