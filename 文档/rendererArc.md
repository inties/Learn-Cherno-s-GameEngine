# GameEngine 渲染器架构文档

## 概述

本文档详细介绍了 GameEngine 的渲染器架构实现。该渲染器采用分层设计，提供平台无关的高级API，同时支持多种图形API（目前实现了OpenGL）。

## 架构层次

### 1. 应用层 (Application Layer)
- 游戏逻辑和用户代码
- 通过高级渲染器API进行渲染调用

### 2. 高级渲染层 (High-Level Renderer)
- `Renderer` 类：提供场景管理和渲染流程控制

### 3. 渲染抽象层 (Renderer Abstraction)
- `Buffer`、`VertexArray`、`Shader` 等抽象接口
- `RenderCommand`、`RendererAPI` 底层抽象

### 4. 平台实现层 (Platform Implementation)
- OpenGL 具体实现：`OpenGLBuffer`、`OpenGLVertexArray`、`OpenGLShader` 等

---

## 核心类详解

## 1. 缓冲区系统 (Buffer System)

### 1.1 ShaderDataType 枚举

```cpp
enum class ShaderDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};
```

**功能**: 定义着色器数据类型，确保类型安全的顶点属性设置。

**主要函数**:
- `ShaderDataTypeSize(ShaderDataType type)`: 返回数据类型的字节大小

### 1.2 BufferElement 结构体

```cpp
struct BufferElement
{
    std::string Name;           // 属性名称
    ShaderDataType Type;        // 数据类型
    uint32_t Size;             // 字节大小
    size_t Offset;             // 在缓冲区中的偏移量
    bool Normalized;           // 是否需要归一化
};
```

**主要函数**:
- `BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)`: 构造函数
- `GetComponentCount()`: 返回组件数量（如 Float3 返回 3，Mat4 返回 16）

### 1.3 BufferLayout 类

**功能**: 描述顶点缓冲区的数据布局，支持多种数据类型的混合。

**主要函数**:
```cpp
// 构造函数，使用初始化列表定义布局
BufferLayout(const std::initializer_list<BufferElement>& elements);

// 获取步长（一个顶点的总字节数）
uint32_t GetStride() const;

// 获取所有元素
const std::vector<BufferElement>& GetElements() const;

// 迭代器支持（支持 range-based for 循环）
std::vector<BufferElement>::iterator begin();
std::vector<BufferElement>::iterator end();
```

**使用示例**:
```cpp
BufferLayout layout = {
    { ShaderDataType::Float3, "a_Position" },
    { ShaderDataType::Float4, "a_Color" },
    { ShaderDataType::Float2, "a_TexCoord" }
};
```

### 1.4 VertexBuffer 抽象类

**功能**: 顶点缓冲区抽象接口，存储顶点数据。

**主要函数**:
```cpp
// 绑定/解绑缓冲区
virtual void Bind() const = 0;
virtual void Unbind() const = 0;

// 动态设置数据
virtual void SetData(const void* data, uint32_t size) = 0;

// 布局管理
virtual const BufferLayout& GetLayout() const = 0;
virtual void SetLayout(const BufferLayout& layout) = 0;

// 工厂方法
static std::shared_ptr<VertexBuffer> Create(uint32_t size);                    // 动态缓冲区
static std::shared_ptr<VertexBuffer> Create(float* vertices, uint32_t size);   // 静态缓冲区
```

### 1.5 IndexBuffer 抽象类

**功能**: 索引缓冲区抽象接口，存储顶点索引以优化绘制。

**主要函数**:
```cpp
// 绑定/解绑缓冲区
virtual void Bind() const = 0;
virtual void Unbind() const = 0;

// 获取索引数量
virtual uint32_t GetCount() const = 0;

// 工厂方法
static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count);
```

### 1.6 OpenGLVertexBuffer 实现类

**功能**: VertexBuffer 的 OpenGL 具体实现。

**主要函数**:
```cpp
// 构造函数
OpenGLVertexBuffer(uint32_t size);                    // 动态缓冲区
OpenGLVertexBuffer(float* vertices, uint32_t size);   // 静态缓冲区

// 实现基类接口
void Bind() const override;
void Unbind() const override;
void SetData(const void* data, uint32_t size) override;
```

**实现细节**:
- 使用 `glCreateBuffers()` 创建缓冲区对象
- 动态缓冲区使用 `GL_DYNAMIC_DRAW`，静态缓冲区使用 `GL_STATIC_DRAW`
- 通过 `glBufferSubData()` 支持数据更新

### 1.7 OpenGLIndexBuffer 实现类

**功能**: IndexBuffer 的 OpenGL 具体实现。

**主要函数**:
```cpp
// 构造函数
OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

// 实现基类接口
void Bind() const override;
void Unbind() const override;
uint32_t GetCount() const override;
```

---

## 2. 顶点数组系统 (Vertex Array System)

### 2.1 VertexArray 抽象类

**功能**: 顶点数组对象抽象接口，封装顶点数据的所有状态。

**主要函数**:
```cpp
// 绑定/解绑 VAO
virtual void Bind() const = 0;
virtual void Unbind() const = 0;

// 添加顶点缓冲区（可以添加多个）
virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

// 设置索引缓冲区（只能有一个）
virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

// 获取缓冲区
virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

// 工厂方法
static std::shared_ptr<VertexArray> Create();
```

### 2.2 OpenGLVertexArray 实现类

**功能**: VertexArray 的 OpenGL 具体实现，管理 VAO 状态。

**主要函数**:
```cpp
// 构造函数和析构函数
OpenGLVertexArray();
virtual ~OpenGLVertexArray();

// 实现基类接口
void Bind() const override;
void Unbind() const override;
void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
```

**核心实现 - AddVertexBuffer**:
```cpp
void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    // 检查缓冲区是否有有效布局
    if (vertexBuffer->GetLayout().GetElements().size() == 0) {
        ENGINE_CORE_ERROR("Vertex Buffer has no layout!");
        return;
    }

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout)
    {
        switch (element.Type)
        {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        {
            // 浮点数属性
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool:
        {
            // 整数属性
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribIPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
        {
            // 矩阵属性（需要拆分为多个向量）
            uint8_t count = element.GetComponentCount();
            uint8_t vectorSize = element.Type == ShaderDataType::Mat3 ? 3 : 4;
            for (uint8_t i = 0; i < count / vectorSize; i++)
            {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                    vectorSize,
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(element.Offset + sizeof(float) * vectorSize * i));
                glVertexAttribDivisor(m_VertexBufferIndex, 1);  // 实例化渲染
                m_VertexBufferIndex++;
            }
            break;
        }
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}
```

**特殊处理**:
- **矩阵属性**: Mat3 和 Mat4 被拆分为多个 vec3/vec4 属性
- **整数属性**: 使用 `glVertexAttribIPointer` 而不是 `glVertexAttribPointer`
- **实例化**: 矩阵属性自动设置为实例化属性

---

## 3. 着色器系统 (Shader System)

### 3.1 Shader 抽象类

**功能**: 着色器程序抽象接口，支持编译、绑定和 uniform 管理。

**主要函数**:
```cpp
// 绑定/解绑着色器
virtual void Bind() const = 0;
virtual void Unbind() const = 0;

// Uniform 设置
virtual void SetInt(const std::string& name, int value) = 0;
virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
virtual void SetFloat(const std::string& name, float value) = 0;
virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

// 获取着色器名称
virtual const std::string& GetName() const = 0;

// 工厂方法
static std::shared_ptr<Shader> Create(const std::string& filepath);
static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
```

### 3.2 OpenGLShader 实现类

**功能**: Shader 的 OpenGL 具体实现，支持从文件和字符串创建着色器。

**主要函数**:
```cpp
// 构造函数
OpenGLShader(const std::string& filepath);                                           // 从文件加载
OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);  // 从源码创建

// 实现基类接口
void Bind() const override;
void Unbind() const override;
void SetInt(const std::string& name, int value) override;
// ... 其他 Set 方法

// 内部 Upload 方法（更底层的 uniform 设置）
void UploadUniformInt(const std::string& name, int value);
void UploadUniformFloat(const std::string& name, float value);
void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
```

**核心实现 - 文件解析**:
```cpp
std::string OpenGLShader::ReadFile(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
    }
    return result;
}
```

**着色器预处理**:
```cpp
std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    
    while (pos != std::string::npos)
    {
        // 查找行结束
        size_t eol = source.find_first_of("\r\n", pos);
        
        // 提取着色器类型
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        
        // 查找下一个着色器类型或文件结束
        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        
        // 提取着色器源码
        shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) 
            ? source.substr(nextLinePos) 
            : source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}
```

**支持的着色器文件格式**:
```glsl
#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
void main()
{
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}
```

---

## 4. 渲染API抽象层 (Renderer API Abstraction)

### 4.1 RendererAPI 抽象类

**功能**: 底层渲染API抽象，定义最基本的渲染操作。

**主要函数**:
```cpp
// API 类型枚举
enum class API { None = 0, OpenGL = 1 };

// 初始化和视口设置
virtual void Init() = 0;
virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

// 清屏操作
virtual void SetClearColor(const glm::vec4& color) = 0;
virtual void Clear() = 0;

// 绘制操作
virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

// 静态方法
static API GetAPI() { return s_API; }
static std::unique_ptr<RendererAPI> Create();
```

### 4.2 OpenGLRendererAPI 实现类

**功能**: RendererAPI 的 OpenGL 具体实现。

**主要函数**:
```cpp
void Init() override
{
    glEnable(GL_BLEND);                               // 启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 设置混合函数
    glEnable(GL_DEPTH_TEST);                          // 启用深度测试
}

void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override
{
    glViewport(x, y, width, height);
}

void SetClearColor(const glm::vec4& color) override
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Clear() override
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) override
{
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}
```

### 4.3 RenderCommand 类

**功能**: 渲染命令调度器，将高级渲染命令转发给当前的 RendererAPI。

**主要函数**:
```cpp
// 初始化
static void Init() { s_RendererAPI->Init(); }

// 视口设置
static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    s_RendererAPI->SetViewport(x, y, width, height);
}

// 清屏操作
static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
static void Clear() { s_RendererAPI->Clear(); }

// 绘制操作
static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0)
{
    s_RendererAPI->DrawIndexed(vertexArray, indexCount);
}
```

**设计优势**:
- **解耦**: 避免在引擎各处直接调用 RendererAPI
- **统一接口**: 所有渲染命令通过统一的静态接口调用
- **易于扩展**: 可以轻松添加命令队列、批处理等功能

---

## 5. 高级渲染器 (High-Level Renderer)

### 5.1 Renderer 类

**功能**: 提供高级渲染接口，管理渲染流程和场景数据。

**主要函数**:
```cpp
// 初始化和清理
static void Init();
static void Shutdown();

// 窗口事件处理
static void OnWindowResize(uint32_t width, uint32_t height);

// 场景渲染流程
static void BeginScene();    // 开始场景渲染
static void EndScene();      // 结束场景渲染

// 提交渲染对象
static void Submit(const std::shared_ptr<Shader>& shader, 
                  const std::shared_ptr<VertexArray>& vertexArray, 
                  const glm::mat4& transform = glm::mat4(1.0f));

// 获取当前API
static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
```

**实现细节**:
```cpp
void Renderer::Init()
{
    RenderCommand::Init();  // 初始化底层渲染API
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, 
                     const std::shared_ptr<VertexArray>& vertexArray, 
                     const glm::mat4& transform)
{
    shader->Bind();           // 绑定着色器
    vertexArray->Bind();      // 绑定顶点数组
    RenderCommand::DrawIndexed(vertexArray);  // 执行绘制
}
```

**场景数据结构**:
```cpp
struct SceneData
{
    glm::mat4 ViewProjectionMatrix;  // 视图投影矩阵
    // TODO: 添加光照、雾效等场景参数
};
```

---

## 6. 使用示例

### 6.1 基本三角形渲染

```cpp
#include "Engine.h"

class TestLayer : public Engine::Layer
{
public:
    void OnAttach() override
    {
        // 1. 创建顶点数据
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  // 左下角 - 红色
             0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  // 右下角 - 绿色
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f   // 顶部 - 蓝色
        };

        uint32_t indices[] = { 0, 1, 2 };

        // 2. 创建顶点缓冲区
        m_VertexBuffer = Engine::VertexBuffer::Create(vertices, sizeof(vertices));
        m_VertexBuffer->SetLayout({
            { Engine::ShaderDataType::Float3, "a_Position" },
            { Engine::ShaderDataType::Float4, "a_Color" }
        });

        // 3. 创建索引缓冲区
        m_IndexBuffer = Engine::IndexBuffer::Create(indices, 3);

        // 4. 创建顶点数组
        m_VertexArray = Engine::VertexArray::Create();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // 5. 创建着色器
        std::string vertexSrc = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;
            out vec4 v_Color;
            void main()
            {
                v_Color = a_Color;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core
            in vec4 v_Color;
            out vec4 color;
            void main()
            {
                color = v_Color;
            }
        )";

        m_Shader = Engine::Shader::Create("TriangleShader", vertexSrc, fragmentSrc);
    }

    void OnUpdate() override
    {
        // 6. 渲染循环
        Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Engine::RenderCommand::Clear();

        Engine::Renderer::BeginScene();
        Engine::Renderer::Submit(m_Shader, m_VertexArray);
        Engine::Renderer::EndScene();
    }

private:
    std::shared_ptr<Engine::VertexBuffer> m_VertexBuffer;
    std::shared_ptr<Engine::IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Engine::VertexArray> m_VertexArray;
    std::shared_ptr<Engine::Shader> m_Shader;
};
```

### 6.2 从文件加载着色器

```cpp
// 着色器文件: assets/shaders/BasicColor.glsl
#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
in vec4 v_Color;
out vec4 color;

void main()
{
    color = v_Color;
}

// C++ 代码
auto shader = Engine::Shader::Create("assets/shaders/BasicColor.glsl");
shader->Bind();
shader->SetMat4("u_ViewProjection", viewProjectionMatrix);
shader->SetMat4("u_Transform", transformMatrix);
```

---

## 7. 架构优势

### 7.1 平台无关性
- **抽象接口**: 所有渲染操作通过抽象接口定义
- **工厂模式**: 运行时选择具体实现
- **易于扩展**: 添加新的图形API只需实现接口

### 7.2 类型安全
- **强类型枚举**: `ShaderDataType` 确保类型安全
- **智能指针**: 自动内存管理，避免内存泄漏
- **RAII**: 资源在析构函数中自动释放

### 7.3 性能优化
- **批处理友好**: 架构支持未来的批处理渲染
- **状态缓存**: VAO 缓存顶点属性状态
- **内存效率**: 使用索引缓冲区减少顶点重复

### 7.4 易用性
- **高级API**: `Renderer` 类提供简单的渲染接口
- **灵活布局**: `BufferLayout` 支持复杂的顶点数据结构
- **统一管理**: `RenderCommand` 统一所有渲染命令

---

## 8. 未来扩展计划

### 8.1 即将添加的功能
- **纹理系统**: `Texture` 抽象和 OpenGL 实现
- **材质系统**: `Material` 类管理着色器和纹理
- **摄像机系统**: 正交和透视投影支持
- **模型加载**: 集成 Assimp 库

### 8.2 性能优化
- **批处理渲染**: 减少绘制调用
- **实例化渲染**: 高效渲染大量相同对象
- **命令队列**: 多线程渲染支持

### 8.3 高级特性
- **阴影映射**: 实时阴影系统
- **后处理**: 屏幕空间效果
- **PBR 渲染**: 基于物理的渲染管线

---

本文档将随着引擎的发展持续更新，确保与实际代码实现保持同步。
