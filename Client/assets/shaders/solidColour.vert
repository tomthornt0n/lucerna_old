#version 330 core

layout(location=0) in vec2  a_Position;
layout(location=1) in vec4  a_Colour;

uniform mat4 u_ViewProjectionMatrix;

out vec4 v_Colour;

void main()
{
    v_Colour = a_Colour;
	gl_Position = u_ViewProjectionMatrix * vec4(a_Position, 0.0, 1.0);
}
