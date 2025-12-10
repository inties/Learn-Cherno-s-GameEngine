

# **渲染器与 ImGui 集成 – 设计建议**

## 1\. 架构概览

`Layer`（层，参见 `Engine/Layer.h` 文件）已经提供了一种模块化的方式，可以将各个子系统插入到核心 `Application`（应用程序）的主循环中（通过 `OnAttach`, `OnDetach`, `OnUpdate`, `OnImGuiRender`, `OnEvent` 等函数）。现代引擎通常会为渲染和UI设置**两个专门的层**：

1.  **RendererLayer (渲染层)** – 拥有所有的 GPU 资源和绘制调用 (draw-calls)。
2.  **ImGuiLayer (ImGui层)** – 负责初始化 Dear ImGui 上下文，将引擎事件转换 → ImGui 的 IO（输入/输出），并**在最后进行渲染**。

> **核心思想**：RendererLayer 应该渲染**场景**对象；而 ImGuiLayer 应该绘制**编辑器/覆盖层**的UI控件。

-----

## 2\. 两层之间的关系

```
Application (应用程序)
 ├─ RendererLayer (渲染层)   (负责更新/提交/呈现画面)
 └─ ImGuiLayer (ImGui层)     (负责开始一帧 → 绘制UI控件 → 结束一帧)
```

  * **RendererLayer (渲染层) 不依赖**于 ImGuiLayer (ImGui层)。
  * **ImGuiLayer (ImGui层) 通过暴露的句柄** (handles)（例如指针/引用、单例或 ECS 查询）从 RendererLayer **拉取数据**。
  * 在 `Application::Run()` 中的执行顺序：
    1.  遍历所有层 → 调用 `OnUpdate()` (此阶段 RendererLayer 填充指令缓冲 Command-buffers)。
    2.  开始 ImGui 帧 (调用 `ImGuiLayer::Begin`)。
    3.  再次遍历所有层 → 调用 `OnImGuiRender()` (此阶段 RendererLayer 发布其控制选项，ImGuiLayer 显示其自身的演示窗口等)。
    4.  调用 `ImGuiLayer::End` → 使用 `RendererAPI` 渲染 ImGui 生成的绘制数据。

-----

## 3\. 将渲染器变量暴露给 ImGui


1.  **公共配置结构体 (Public Config Struct)**

    ```cpp
    struct RendererSettings {
        glm::vec3 clearColor{0.1f, 0.1f, 0.1f}; // 清屏颜色
        bool vsync = true;                      // 垂直同步
        bool wireframe = false;                 // 线框模式
    };
    ```

    这个结构体存储在 `RendererLayer` 内部，通过 `GetSettings()` 函数返回其**引用**。



## 5\. 在 RendererLayer 中绘制imgui?



  * 使用现有的 **`OnImGuiRender()`** 回调。
  * 将渲染相关的逻辑保留在 `OnUpdate()` / `Submit()` 中。
  * 在 `OnImGuiRender()` 内部暴露运行时的统计数据（如帧率 FPS、绘制调用次数、GPU 计时器等），目前只绘制一个imgui文本以测试功能。




**预期成果:**

  * **松散耦合 (Loose coupling)**；渲染器可以在没有 ImGui 的情况下“无头”(headless) 运行。
  * ImGui 控件可以**实时**调整渲染器的状态。