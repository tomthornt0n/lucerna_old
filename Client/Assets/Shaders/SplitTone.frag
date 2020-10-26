#version 330 core

layout(location=0) out vec4 o_colour;

in vec4 v_Colour;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

vec4 shadows = vec4(0.0, 0.2, 0.6, 1.0);
vec4 highlights = vec4(0.6, 0.2, 0.0, 1.0);
float splitToneIntensity = 0.2;

void main()
{

    vec4 col = texture2D(u_Texture, v_TexCoord) * v_Colour;

    if (col.a < 0.5)
    {
        discard;
    }

    /*
    if (gl_FragCoord.x < 960)
    {
        */
        float mixFactor = ((col.r + col.g + col.b) / 3);

        col = mix(col, highlights, mixFactor * splitToneIntensity);
        col = mix(col, shadows, (1.0 - mixFactor) * splitToneIntensity);
    /*
    }
    */
    o_colour = col;
}
