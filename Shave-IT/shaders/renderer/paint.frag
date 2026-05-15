#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D UVTexture;
uniform sampler2D PaintTexture;
uniform vec2 MouseUV;


void main()
{
    //normalize mouse from [-1 to 1] to [0 to 1] 
    vec2 screenUV = gl_FragCoord.xy / vec2(textureSize(UVTexture, 0));

    // UV of model at this pixel
    vec2 uv = texture(UVTexture, screenUV).xy;

    // brush in screen space
    float dist = distance(uv, MouseUV);

    float radius = 0.03;

    if (dist < radius)
    {
        FragColor = vec4(uv, 0, 1); // debug
    }
    else
    {
        discard;
    }
}