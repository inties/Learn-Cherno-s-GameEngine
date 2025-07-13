# 着色器文件说明

这个文件夹包含了 GameEngine 渲染器使用的着色器文件。

## 文件结构

### 顶点着色器
- `vertex_shader.glsl` - 通用顶点着色器
  - 接收顶点位置 (a_Position) 和颜色 (a_Color)
  - 支持变换矩阵 (u_Transform)
  - 输出位置和颜色到片段着色器

### 片段着色器
- `fragment_position.glsl` - 基于位置的片段着色器
  - 根据顶点位置生成颜色
  - 产生渐变效果

- `fragment_color.glsl` - 基于顶点颜色的片段着色器
  - 直接使用顶点数据中的颜色
  - 显示纯色三角形

## 使用方法

在 TestLayer.cpp 中，通过以下方式加载着色器：

```cpp
// 位置颜色着色器
m_Shader = Engine::Shader::CreateFromFiles("VertexPosColor", 
    "GameEngine/Shader/vertex_shader.glsl", 
    "GameEngine/Shader/fragment_position.glsl");

// 顶点颜色着色器
m_BlueShader = Engine::Shader::CreateFromFiles("VertexColor", 
    "GameEngine/Shader/vertex_shader.glsl", 
    "GameEngine/Shader/fragment_color.glsl");
```

## 着色器特性

- OpenGL 3.3 Core Profile 兼容
- 支持变换矩阵
- 支持顶点属性（位置 + 颜色）
- 可在运行时切换不同的着色器 