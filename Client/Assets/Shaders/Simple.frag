#version 330 core

layout(location=0) out vec4 o_colour;

in vec4 v_Colour;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
    vec4 col = texture2D(u_Texture, v_TexCoord) * v_Colour;
    if (col.a < 0.5)
    {
        discard;
    }
    o_colour = col;
}
