
#version 420 core

// 输出的最终颜色
out vec4 FragColor;

// 从顶点着色器传入的纹理坐标
in vec2 TexCoords;

// uniform 变量，代表屏幕纹理采样器
layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
    // 使用传入的纹理坐标对屏幕纹理进行采样
    vec3 color = texture(u_Texture, TexCoords).rgb;
    color = color / (color + vec3(1.0));
    float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));
    

    
    // 将灰度值作为新的R, G, B分量，保持原始的alpha值不变
    FragColor = vec4(color, 1.0);
}