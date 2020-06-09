#version 330 core

layout(location=0) in vec2  a_Position;
layout(location=1) in vec4  a_Colour;

out vec4 v_Colour;

void main()
{
    v_Colour = a_Colour;
	gl_Position = vec4(a_Position, 0.0, 1.0);
}
