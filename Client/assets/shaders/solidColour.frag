#version 330 core

layout(location=0) out vec4 colour;

in vec4 v_Colour;

void main()
{
	colour = v_Colour;
}
