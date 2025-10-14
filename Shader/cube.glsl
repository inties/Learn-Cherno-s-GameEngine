// 立方体着色器 - 支持位置和颜色
#type vertex
#version 420 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // 传递颜色到片元着色器
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    // 计算最终位置
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 420 core


layout (binding = 0) uniform sampler2D u_Texture;
in vec4 v_Color;
in vec2 v_TexCoord;
out vec4 FragColor;

void main()
{
    // 使用从顶点着色器传递的颜色
    FragColor = texture(u_Texture, v_TexCoord);
}