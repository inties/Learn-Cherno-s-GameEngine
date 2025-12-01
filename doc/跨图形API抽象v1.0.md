# 跨图形 API 抽象设计文档

#### 设计目标
- 抽象渲染命令与资源，屏蔽具体后端差异，支持可扩展后端切换
- 轻量化，贴近 OpenGL 使用范式；以最小接口承载常用渲染与计算
- 状态以描述符统一表达（混合、深度模板、清屏），便于跨后端映射

#### 整体架构
- 命令分层：`RenderCommand` 封装引擎对外命令接口 → 委派到 `RendererAPI` → 具体后端实现（当前为 OpenGL）
- 资源工厂：抽象资源类提供 `Create`/`CreateScope`/`CreateRef` 等工厂，根据当前 `Renderer::GetAPI()` 产出后端具体类型

![跨平台架构图](C:\Users\邓力源\Desktop\跨平台架构图.png)


#### 资源抽象

##### 设计要点

- 资源创建：统一通过抽象工厂按 `Renderer::GetAPI()` 分发到后端类，资源使用 RAII 管理（构造加载、析构释放）。
- 绑定：资源都暴露 `Bind`方法 ，按照OpenGL风格绑定；并暴露底层 `GetRendererID()` 便于与 FBO/SSBO 等 API 交互。
- 属性枚举：通过 `TextureFormat`、`TextureAccess`、`BufferLayout`、`TopologyType` 等枚举与结构统一描述资源属性。
- 生命周期：`Ref`/`Scope`/`std::shared_ptr` 统一所有权；工厂仅返回智能指针。
- 可扩展性：抽象接口稳定，后端新增时仅需在工厂与 `RendererAPI` 实现对应方法。


##### 各资源封装方案

- Texture/Texture2D/TextureCube
  - 思路/功能：统一纹理抽象，支持采样与计算读写；格式由 `TextureFormat` 描述。
  - 核心方法：`Bind(slot)`、`BindAsImage(ImageBindDesc)`、`SetData(data,size)`、`Clear()`、`GetWidth/Height/RendererID`。
  - 说明：`BindAsImage(ImageBindDesc)`将纹理绑定为计算着色器可写入的资源（opengl的image）。
  - 设计优点/不足：未使用纹理描述结构体来统一描述纹理，不方便扩展成DX12等现代API
  - 后端实现：
```cpp
// GameEngine/src/Engine/Renderer/Texture.cpp
Ref<Texture2D> Texture2D::Create(uint32_t w,uint32_t h,TextureFormat fmt,int sample){
  switch(Renderer::GetAPI()){
    case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(w,h,fmt,sample);
    default: ENGINE_CORE_ASSERT(false,"RendererAPI::None is currently not supported!"); return nullptr;
  }
}
```
```cpp
// GameEngine/src/Engine/platform/OpenGL/OpenGLTexture.cpp
void OpenGLTexture2D::BindAsImage(const ImageBindDesc& desc)const{
  GLenum accessGL = desc.access==TextureAccess::ReadOnly?GL_READ_ONLY:
                    desc.access==TextureAccess::WriteOnly?GL_WRITE_ONLY:GL_READ_WRITE;
  glBindImageTexture(desc.binding,m_RendererID,desc.mipLevel,
                     desc.layered?GL_TRUE:GL_FALSE,desc.layer,
                     accessGL,m_InternalFormat);
}
```
​	

- Framebuffer
  - 思路/功能：以规格 `FramebufferSpecification` 构建颜色/深度附件，支持读像素、清除与 Blit。
  - 核心方法：`Bind/Unbind`、`Resize`、`ReadPixel`、`ClearAttachment(s)`、`ColorMask`、`GetRenderTexture/GetDepth`。
  - 说明：framebuffer自己创建并管理Texture2D，并在resize时重新创建Texture2D。framebuffer提供接口访问自身的纹理附件。
  - 优缺点/改进：framebuffer和纹理关系较简单，但两者耦合严重。framebuffer作为一个容器，只引用纹理，并在resize时调用纹理的resize函数会更合理。
  - 后端实现：

```cpp
// GameEngine/src/Engine/platform/OpenGL/OpenGLFrameBuffer.cpp
glCreateFramebuffers(1,&m_FBOID);
glBindFramebuffer(GL_FRAMEBUFFER,m_FBOID);
Ref<Texture> color = Texture2D::Create(m_Specification.Width,m_Specification.Height,fmt,1);
glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,color->GetRendererID(),0);
ENGINE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE,"Framebuffer is incomplete!");
```


- Buffer
  - 思路/功能：`VertexBuffer/IndexBuffer` 管理顶点和索引缓冲。VertexBuffer通过BufferLayout成员描述其顶点布局，并可通过SetData上传cpu数据到显存

  - 核心方法：`SetData`、`Bind`、`GetSize/Count`；

    

  - VertexArray

    - 思路/功能：借用Opengl的VAO概念，这个类封装绘制命令所需的所有几何数据，包括引用顶点缓冲和索引缓冲，以及顶点布局。支持多个物体的顶点和索引缓冲合并。
    - 核心方法： `SetVertexBuffer/SetIndexBuffer`、`GetVertexCount`

    

- Shader
  - 思路/功能：封装 program 的创建与 uniform 设置。平台具体类如Openglshader的构造函数封装了shader的编译链接过程，能基于源文件构建shader对象。
  - 核心方法：`Bind/Unbind`、`SetInt/Float/FloatN/Mat4`、多种 `Create` 入口。
  - 优点/不足：接口统一；按照opengl风格，通过shader programm

```cpp
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        ...
            
    }
		OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}
```


- Material
  - 思路/功能：聚合 Shader 与 Texture，统一一次性绑定材质所需资源。
  - 核心方法：`Bind/Unbind`、`SetShader`、`SetTexture`、`SetFloat/Vec/Mat4`。
  - 优点/不足：批量绑定易用；渲染器可按材质合批，减少状态切换。缺点：未支持UBO，材质级uniform设置依赖于Opengl中uniform变量依附于shader program的特性。
  - 代码示例：

```cpp
class OpenGLMaterial{
    public:
    void Bind() const override;
    void Unbind() const override;
    Ref<Shader> GetShader() const override { return m_Shader; }
    ...
    void SetFloat4(const std::string& name, const glm::vec4& value) override;
    void SetMat4(const std::string& name, const glm::mat4& value) override;
    void SetTexture(const std::string& name, const Ref<Texture>& texture,int slot) override;
    ...
    private:
    std::unordered_map<std::string, std::pair<Ref<Texture>,int>> m_Textures;
    Ref<Shader> m_Shader;
}
```


- SSBO（ShaderStorageBuffer）
  - 思路/功能：计算/图形共享大数据缓冲。
  - 核心方法：`Bind(slot)`、`SetData(data,size,offset)`，EnsureCapacity
  - 代码示例：
  - 优点/不足：高带宽；需正确屏障与格式协议。



#### 命令架构与典型命令

##### 设计要点

- 直接命令封装：以 `RenderCommand` 静态方法统一入口，内部委派给 `RendererAPI`，并切换到具体RenderAPI类型例如OpenglRenderAPI。

- 状态描述符：通过 `BlendDesc`、`DepthStencilDesc`、`ClearDesc` 精细地控制渲染状态。

- 典型命令包括：drawArray\drawIndex、dispatch、InsertBarrier、clear、setblendstate等

- 缺点：未封装Pipelinestate，渲染状态的切换仍然使用opengl风格

  

**代码示例**

```cpp
状态切换：
// GameEngine/src/Engine/Renderer/RenderCommand.h
RenderCommand::SetBlendState(BlendDesc::CreateAlphaBlend());
RenderCommand::SetDepthStencilState(DepthStencilDesc::Default());
RenderCommand::DrawIndexed(vertexArray);

计算派发与屏障：
RenderCommand::InsertBarrier(BarrierDomain::ComputeWriteToComputeRead);
RenderCommand::Dispatch(8,8,1);

清屏：
ClearDesc cd = ClearDesc::Default();
cd.Color = {0.1f,0.1f,0.12f,1.0f};
RenderCommand::Clear(cd);
```






- RenderCommand 封装：视口、清屏、绘制、状态设置、计算派发、屏障、FBO blit
- RendererAPI 提供统一接口与状态描述符：
  - 清屏：Clear(const ClearDesc&) 可选择性清颜色/深度/模板
  - 绘制：DrawIndexed/DrawArrays/DrawIndexedWithOffset/DrawIndexedInstanced
  - 计算：Dispatch
  - 屏障：InsertBarrier(BarrierDomain) 用域抽象典型跨阶段依赖
  - 状态：SetBlendState(const BlendDesc&)、SetDepthStencilState(const DepthStencilDesc&)
  - FBO 拷贝：BlitFramebuffer 颜色/深度可选
- 状态描述符
  - 混合：BlendDesc/RenderTargetBlendDesc 支持独立RT与常用预设
  - 深度/模板：DepthStencilDesc 与 StencilOpState

示例用法

```cpp
using namespace Engine;
RenderCommand::SetViewport(0,0,w,h);
ClearDesc cd = ClearDesc::Default();
cd.Color = {0.1f,0.1f,0.12f,1.0f};
RenderCommand::Clear(cd);
RenderCommand::SetBlendState(BlendDesc::CreateAlphaBlend());
RenderCommand::SetDepthStencilState(DepthStencilDesc::Default());
RenderCommand::Dispatch(8,8,1);
RenderCommand::InsertBarrier(BarrierDomain::ComputeWriteToGraphicsRead);
```



#### OpenGL 后端实现要点

- 使用DSA方式进行资源绑定（直接资源访问）
```cpp
// GameEngine/src/Engine/platform/OpenGL/OpenGLTexture.cpp
glCreateTextures(GL_TEXTURE_2D,1,&m_RendererID);
glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Width, m_Height);
glBindTextureUnit(slot, m_RendererID);
```
说明：DSA 直连对象，无需先绑定到当前上下文，减少状态污染与切换开销。





#### 不足与改进方向
- OpenGL 概念外露：VAO、SSBO 等名词直接体现在抽象层，迁移到其他 API 难度较大
- 未抽象 UBO/Sampler/DescriptorSet 等现代图形对象；常量与绑定仍依赖 glUniform
- 能力/版本未做查询与降级路径











