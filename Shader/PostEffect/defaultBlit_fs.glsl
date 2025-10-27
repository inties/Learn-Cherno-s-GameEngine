#version 420 core

// 输出的最终颜色
layout(location =0)out vec4 FragColor;
layout(location =1)out vec4 BloomColor;
// 从顶点着色器传入的纹理坐标
in vec2 TexCoords;

// uniform 变量，代表屏幕纹理采样器
layout(binding = 0) uniform sampler2D u_Texture;



void main()
{
   vec4 color = texture(u_Texture, TexCoords);
   
   // 计算亮度 (使用标准亮度公式)
   float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
   
   // 主输出
   FragColor = color;
   
   // 如果亮度 > 1.0，输出到 BloomColor
   if (brightness > 1.0) {
       BloomColor = color;
   } else {
       BloomColor = vec4(0.0);
   }
}


