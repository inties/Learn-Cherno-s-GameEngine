#version 420 core

// 输出的最终颜色
out vec4 FragColor;

// 从顶点着色器传入的纹理坐标
in vec2 TexCoords;

// uniform 变量，代表屏幕纹理采样器
layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
   FragColor = texture(u_Texture, TexCoords);
}


