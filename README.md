# ReadMe

该项目是一个基于 C++ 与 OpenGL 的实时渲染器，实现了 Forward+ 渲染管线，能够在包含大量动态光源（1000+）与大规模实例化对象（2000+）的场景中保持稳定高帧率。系统使用现代 OpenGL 技术（DSA、SSBO、compute shader），实现了编辑器、简单的资源管理、ECS架构以及可扩展的跨平台后端接口。

更多文档参考：

[跨平台API抽象文档](doc/跨图形 API 抽象设计文档.md)

[Forward+](doc/Forward+.md)

------

###  主要特点

##### 现代图形 API 使用

- 使用 Direct State Access（DSA）管理 GPU 资源，避免全局状态污染。
- 使用 SSBO 及 bindless textures 实现动态索引与高效材质访问。
- 使用 memory barrier 确保 GPU 阶段间的数据可见性与执行顺序。
- 使用 compute shader 实现 Forward+ 光源剔除与部分后处理效果。

  

##### 系统架构与工程设计

- 设计了跨平台渲染后端抽象，统一封装 Texture、Buffer、Shader、Framebuffer 等资源类型，并可扩展 DX12/Vulkan 等 API。参考[跨平台API抽象文档](doc/跨图形 API 抽象设计文档.md)
- 集成 entt 实现 ECS 架构，渲染组件与渲染器模块解耦。
- 构建资源注册表，用于统一加载、缓存和管理 GPU 资源。

##### 编辑器与调试

- 基于 ImGui 构建轻量级编辑器，支持对象选取、平移、旋转等基础操作。
- 实时显示帧率等调试信息。
- 支持运行时调节材质及光照参数，便于测试和调试。

##### Forward+ 渲染管线

- 包含 pre-z、不透明、天空盒、半透明、后处理等渲染阶段。
- 使用 compute shader 实现 **光源剔除**，支持大量动态光源。
- 在高负载场景下显著降低像素着色阶段的光照计算开销。

##### 性能优化

- 基于材质与几何信息的**自动合批**，进行实例化绘制。减少drawcall的数目。
- 使用 **Weighted Blend OIT** 完成无需排序的透明渲染。
- 在 1000+ 动态光源与 2000+ 实例化对象的场景中保持良好帧率。

##### 渲染算法

- 基于 Metallic-Roughness 模型实现的 PBR。
- IBL 环境光照。
- Weighted Blend OIT 透明渲染。

------

###  未来开发计划

- 增加DX12后端
- 性能优化：引入多线程渲染，GPU-driven pipeline
- 渲染效果：集成全局光照算法

- 继续集成 RSM、SSDO 等全局光照算法。