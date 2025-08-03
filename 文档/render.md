### 渲染器设计方案 (增强版)

#### 1. 核心设计理念

渲染器的目标是提供一个**平台无关**的高级API，供应用程序（例如 `SandBox`）调用，同时将所有平台相关的底层实现（如 OpenGL, Vulkan, DirectX）完全封装起来。

我们将渲染系统划分为四个主要层次：

1.  **应用层**: 游戏或应用逻辑。
2.  **高级渲染层**: 提供 `BeginScene`/`EndScene` 这样的高级接口，管理渲染流程。
3.  **渲染抽象层**: 定义平台无关的渲染对象接口，如 `VertexBuffer`, `Shader`, `Texture`。
4.  **平台实现层**: 对渲染抽象接口的具体实现，如 `OpenGLVertexBuffer`。



#### 3. 核心组件详解

##### 3.1. 高级渲染层 (`Renderer`)

这是渲染系统的主要入口。它提供一个简单的API来控制整个渲染流程。

-   **`Renderer` 类**:
    -   `static void Init()`: 初始化渲染API。
    -   `static void BeginScene(Camera& camera)`: 开始一个新场景的渲染，传入摄像机以上传视图和投影矩阵。
    -   `static void EndScene()`: 结束场景渲染，并执行渲染命令队列。
    -   `static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f))`: 提交一个要绘制的对象（由着色器和顶点数组定义）。

##### 3.2. 底层抽象 (`RendererAPI` 和 `RenderCommand`)

这一层将高级 `Renderer` 与具体的图形API（如OpenGL）解耦。

-   **`RendererAPI` (接口)**:
    -   定义了最底层的绘图操作，如 `Init()`, `SetClearColor()`, `Clear()`, `DrawIndexed()`。
    -   将由平台相关的类（如 `OpenGLRendererAPI`）实现。
    -   它也定义了所使用的图形API类型（OpenGL, Vulkan等）。

-   **`RenderCommand` (调度器)**:
    -   一个静态类，它持有 `RendererAPI` 的一个实例。
    -   将所有来自 `Renderer` 的绘图指令转发给当前的 `RendererAPI` 实例。这避免了在引擎各处直接调用 `RendererAPI`。

##### 3.3. 渲染抽象

这些是构成可渲染对象的核心组件的接口。

-   **`VertexBuffer` 和 `IndexBuffer` (接口)**:
    -   `VertexBuffer`: 存储顶点数据（位置、法线、纹理坐标等）。
    -   `IndexBuffer`: 存储顶点索引，用于高效绘制。
    -   职责：创建缓冲区、绑定/解绑、设置数据。

-   **`BufferLayout`**:
    -   一个辅助类，用于描述顶点缓冲区中数据的布局（例如：`float3` for position, `float2` for texcoord）。
    -   这使得 `VertexBuffer` 的创建和使用更加灵活和安全，无需硬编码顶点属性。

-   **`VertexArray` (VAO - 接口)**:
    -   这是至关重要的一个抽象。它封装了顶点数据的所有状态。
    -   一个 `VertexArray` 对象会包含一个或多个 `VertexBuffer` 和一个 `IndexBuffer`。
    -   在OpenGL中，这直接映射到 VAO (Vertex Array Object)。

-   **`Shader` (接口)**:
    -   职责：从文件或字符串加载和编译着色器源码。
    -   提供 `Bind()` / `Unbind()` 方法。
    -   提供用于设置uniforms（如 `mat4`, `vec4`, `int`）的接口，例如 `UploadUniformMat4(...)`。

-   **`Texture` (接口)**:
    -   职责：从文件加载图像数据。
    -   提供 `Bind()` / `Unbind()` 方法，支持绑定到特定的纹理槽。

##### 3.4. 资源/场景管理

-   **`Model` 和 `Mesh`**:
    -   使用 Assimp 这样的库来加载3D模型。
    -   一个 `Model` 由一个或多个 `Mesh` 组成。
    -   每个 `Mesh` 包含一个 `VertexArray` 和一个 `Material`。

-   **`Material` (推荐新增)**:
    -   一个 `Material` 实例定义了一个表面的外观。
    -   它应该持有对 `Shader` 的引用。
    -   它应该管理一组 `Texture`（如漫反射贴图、高光贴图）。
    -   它还存储着色器所需的uniform值（如颜色、粗糙度等）。
    -   `Renderer` 在提交绘制命令时，可以接受一个 `Material` 来配置渲染状态。

-   **`Camera`**:
    -   控制场景的视口和投影。
    -   提供视图矩阵 (`ViewMatrix`) 和投影矩阵 (`ProjectionMatrix`)。
    -   支持正交 (`Orthographic`) 和透视 (`Perspective`) 投影。

#### 4. 优势总结

-   **高度解耦**: 应用逻辑与渲染细节完全分离。
-   **易于扩展**: 添加新的图形API（如Vulkan）只需实现新的平台层，而无需更改上层代码。
-   **代码清晰**: 职责分明，每个类都有单一的、明确的目标。
-   **面向未来**: 为将来实现更复杂的功能（如批处理渲染、多线程渲染）打下了坚实的基础。
