#version 330 core

in vec3 WorldPos;

out vec4 FragColor;

uniform vec3 BrushWorldPos;
uniform float BrushRadius;
uniform float Grow;
uniform float Hair;

void main()
{
    vec2 color = vec2(Grow, Hair);

    if (length(color) > 0){
        FragColor = vec4(color, 0.0, 1.0);
        return;
    }
    
    if (distance(WorldPos, BrushWorldPos) < BrushRadius){
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;    
    }
    FragColor = vec4(0.0);
   
}