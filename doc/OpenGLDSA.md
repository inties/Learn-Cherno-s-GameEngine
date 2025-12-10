## OpenGL DSA

#### 基本概念

DSA（Direct State Access）是Opengl 4.5完整支持的特性，是一种新的OpenGL API调用方式，无需绑定到全局Context即可修改OpenGL对象，从而避免全局状态污染，同时性能没有损失。

由于OpenGL是C风格的API没有函数重载，为了区别于传统API，DSA API的函数名中OpenGL对象名称和传统API有区别。如下表所示（[Direct State Access - OpenGL Wiki](https://wikis.khronos.org/opengl/Direct_State_Access)）：

|                      OpenGL Object Type                      | Context Object Name  |   DSA Object Name   |
| :----------------------------------------------------------: | :------------------: | :-----------------: |
|  [Texture Object](https://wikis.khronos.org/opengl/Texture)  |        "Tex"         |      "Texture"      |
| [Framebuffer Object](https://wikis.khronos.org/opengl/Framebuffer_Object) |    "Framebuffer"     | "NamedFramebuffer"  |
| [Buffer Object](https://wikis.khronos.org/opengl/Buffer_Object) |       "Buffer"       |    "NamedBuffer"    |
| [Transform Feedback Object](https://wikis.khronos.org/opengl/Transform_Feedback_Object) | "TransformFeedback"1 | "TransformFeedback" |
| [Vertex Array Object](https://wikis.khronos.org/opengl/Vertex_Array_Object) |         N/A2         |    "VertexArray"    |
| [Sampler Object](https://wikis.khronos.org/opengl/Sampler_Object) |         N/A3         |      "Sampler"      |
| [Query Object](https://wikis.khronos.org/opengl/Query_Object) |         N/A3         |       "Query"       |
| [Program Object](https://wikis.khronos.org/opengl/Program_Object) |         N/A3         |      "Program"      |




#### 本项目DSA API使用举例

##### 1. 纹理 (Texture)

本项目中 `OpenGLTexture2D` 和 `OpenGLTextureCube` 等纹理资源OpenGL后端实现中，广泛使用了 DSA 特性：

```cpp
// 1. 创建纹理对象 (glCreate*)
glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

// 2. 分配不可变存储空间 (glTextureStorage*)
glTextureStorage2D(m_RendererID, mipLevels, m_InternalFormat, m_Width, m_Height);

//纹理参数设置
glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

// 更新纹理数据
glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

// 生成 Mipmap
glGenerateTextureMipmap(m_RendererID);

//纹理绑定
glBindTextureUnit(slot, m_RendererID);
```


##### 2. 帧缓冲 (Framebuffer)

项目中 `OpenGLFrameBufferCube` 类展示了较为完整的 DSA 帧缓冲操作，而 `OpenGLFramebuffer` 则处于混合状态。

```cpp
// 1. 创建
glCreateFramebuffers(1, &m_FBOID);

// 2. 附加纹理 (无需绑定 Framebuffer)
glNamedFramebufferTextureLayer(m_FBOID, GL_COLOR_ATTACHMENT0, m_textureCube->GetRendererID(), 0, faceID);

// 3. 检查状态
if (glCheckNamedFramebufferStatus(m_FBOID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { ... }

// 4. 清除内容 (glClearNamedFramebuffer*)
// 直接清除指定 Framebuffer 的附件，不依赖全局清除颜色状态
float clearDepth = 1.0f;
glClearNamedFramebufferfv(m_FBOID, GL_DEPTH, 0, &clearDepth);
```

##### 3. 缓冲对象 (Buffers)

目前的 `OpenGLBuffer.cpp` 处于 DSA 过渡阶段。

- **创建**：使用了 `glCreateBuffers(1, &m_RendererID)` (DSA 风格)。
- **数据操作**：仍主要使用 `glBindBuffer` + `glBufferData` (传统风格)。

**完全 DSA 化的改进方向**：
应使用 `glNamedBufferData` 或 `glNamedBufferSubData` 替代绑定操作。

```cpp
// 推荐改进：
glNamedBufferData(m_RendererID, size, data, GL_STATIC_DRAW);
```



#### 注意事项

- DSA 需要 OpenGL 4.5+ 支持。
- 混合使用传统 API (如 `glBindBuffer`) 和 DSA API (`glCreateBuffers`) 是合法的，但应尽量统一以保持代码风格一致性。



