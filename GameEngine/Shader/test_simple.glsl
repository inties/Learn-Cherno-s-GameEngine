// 最简单的测试着色器 - 用于验证基础渲染
#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    // 直接输出顶点位置，不做任何变换（用于测试）
    gl_Position = vec4(a_Position * 0.001, 1.0); // 极度缩小模型
    
    // 正常变换（如果上面不行就用这个）
    // gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;

void main()
{
    // 强制输出红色
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}