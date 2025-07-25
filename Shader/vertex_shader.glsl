#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Transform;

out vec3 v_Position;
out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
	v_Position = a_Position;
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	gl_Position = u_Transform * vec4(a_Position, 1.0);	
} 