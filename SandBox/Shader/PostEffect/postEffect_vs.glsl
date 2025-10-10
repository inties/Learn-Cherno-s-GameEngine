
#version 420 core

// 顶点属性：位置坐标 (x, y)
layout (location = 0) in vec2 aPos;
// 顶点属性：纹理坐标 (u, v)
layout (location = 1) in vec2 aTexCoords;

// 输出到片段着色器的纹理坐标
out vec2 TexCoords;

void main()
{
    // 将纹理坐标直接传递给片段着色器
    TexCoords = aTexCoords;
    // 直接输出顶点位置，因为我们通常提供的是标准化设备坐标(NDC)
    // z = 0.0, w = 1.0
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
