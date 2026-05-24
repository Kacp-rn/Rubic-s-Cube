#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool useTint;
uniform vec3 tintColor;

void main()
{    
    vec4 tex = texture(texture_diffuse1, TexCoords);
    if (useTint)
        FragColor = vec4(tex.rgb * tintColor, tex.a);
    else
        FragColor = tex;
}