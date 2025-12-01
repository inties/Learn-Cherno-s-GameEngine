## OpenGL DSA(直接资源访问)

#### DSA概念介绍

DSA是Opengl 4.x支持的现代特性，是一种新的OpenGL API调用方式，能够避免全局状态污染。应当尽可能地使用DSA方式调用OpenGL API。


#### DSA API举例与项目实践

##### 1. 纹理 (Texture)

项目中 `OpenGLTexture2D` 和 `OpenGLTextureCube` 广泛使用了 DSA 特性。

###### 纹理创建与内存分配

**传统方式**：
```cpp
GLuint id;
glGenTextures(1, &id);
glBindTexture(GL_TEXTURE_2D, id);
glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr); // 同时分配并上传
```

**项目 DSA 方式 (`OpenGLTexture2D.cpp`)**：
```cpp
// 1. 创建纹理对象 (glCreate*)
glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

// 2. 分配不可变存储空间 (glTextureStorage*)
// 显式指定 mipmap 层级和内部格式
glTextureStorage2D(m_RendererID, mipLevels, m_InternalFormat, m_Width, m_Height);
```
*优势*：`glTextureStorage2D` 创建的是不可变存储（Immutable Storage），驱动程序可以提前优化内存布局，且避免了验证纹理完整性的开销。

###### 纹理参数与数据上传

**项目 DSA 方式**：
```cpp
// 直接设置参数，无需绑定
glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

// 上传数据 (glTextureSubImage*)
glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

// 生成 Mipmap
glGenerateTextureMipmap(m_RendererID);
```

###### 纹理绑定

**项目 DSA 方式**：
```cpp
// 将纹理绑定到指定纹理单元 (Unit Slot)
glBindTextureUnit(slot, m_RendererID);
```
这替代了 `glActiveTexture` + `glBindTexture` 的组合。

##### 2. 帧缓冲 (Framebuffer)

项目中 `OpenGLFrameBufferCube` 展示了较为完整的 DSA 帧缓冲操作，而 `OpenGLFramebuffer` 则处于混合状态。

**完全 DSA 示例 (`OpenGLFrameBufferCube`)**：
```cpp
// 1. 创建
glCreateFramebuffers(1, &m_FBOID);

// 2. 附加纹理 (无需绑定 Framebuffer)
// glNamedFramebufferTextureLayer 用于立方体贴图或数组纹理
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

#### DSA 优势总结 (结合项目)

1.  **减少状态切换开销**：例如 `OpenGLTexture2D::Bind` 只需一行 `glBindTextureUnit`，无需切换 `ActiveTexture`。
2.  **代码更直观**：`OpenGLFrameBufferCube` 中针对特定对象的 `glClearNamedFramebuffer` 操作比传统的 `glClear`（依赖全局状态）更安全，尤其是在多 RenderTarget 场景下。
3.  **不可变存储**：`glTextureStorage2D` 避免了纹理完整性检查，这在引擎资源频繁加载时有助于性能。

#### 注意事项

- DSA 需要 OpenGL 4.5+ 支持。
- 混合使用传统 API (如 `glBindBuffer`) 和 DSA API (`glCreateBuffers`) 是合法的，但应尽量统一以保持代码风格一致性。


##### 3. 缓冲对象 (Buffers) 与 顶点数组 (VertexArray)

目前的 `OpenGLBuffer.cpp` 和 `OpenGLVertexArray.cpp` 处于 DSA 过渡阶段。

- **创建**：使用了 `glCreateBuffers` / `glCreateVertexArrays` (DSA 风格)。
- **数据与状态操作**：仍主要使用 `glBindBuffer` / `glBindVertexArray` 配合传统 API (`glBufferData`, `glVertexAttribPointer`)。

**完全 DSA 化的改进方向**：
- **Buffer**: 使用 `glNamedBufferData` 替代 `glBufferData`。
- **VertexArray**: 使用 `glVertexArrayVertexBuffer`, `glVertexArrayAttribFormat`, `glVertexArrayAttribBinding` 替代 `glVertexAttribPointer`。

```cpp
// Buffer 推荐改进：
glNamedBufferData(m_RendererID, size, data, GL_STATIC_DRAW);
```

