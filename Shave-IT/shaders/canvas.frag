#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform int Mirror;
uniform float TrimLength;

uniform sampler2D BrushTexture;
uniform sampler2D PaintTexture;

void main()
{
    vec3 color0 = texture(BrushTexture, TexCoord).rgb;
    vec3 color1 = texture(PaintTexture, TexCoord).rgb;

    if (color0.b > 0.0){
        FragColor = vec4(min(color1.r, TrimLength), min(color1.g, TrimLength), 0.0, 1.0);
        return;
    }
    if (Mirror > 0)
    {
        color0 = texture(BrushTexture, vec2(TexCoord.x, -TexCoord.y)).rgb;
        if (color0.b > 0.0){
            FragColor = vec4(min(color1.r, TrimLength), min(color1.g, TrimLength), 0.0, 1.0);
            return;
        }
    }

    FragColor = vec4(min(color0.r + color1.r, 0.5),clamp(color0.g + color1.g, 0.0, 1.0), 0.0, 1.0);

}
