Editor的功能特点：
- 多个功能面板（Panel），各司其职，提供直观的交互界面。

- 视口窗口（Viewport）：实时展示场景渲染结果，并叠加显示物体的 Gizmos（操作手柄），支持相机导航和物体交互。

- 场景编辑功能：支持创建、删除、移动、缩放、旋转物体，以及拾取（Picking）和多选操作。

- 资源管理功能：导入、导出和管理模型、纹理等资源，支持拖拽导入和路径浏览。

- Inspector 窗口：展示和修改所选物体的属性，包括 Transform（位置/旋转/缩放）、材质参数（颜色/金属度/粗糙度/纹理）和组件扩展（未来 ECS 支持）。

- 资源管理窗口：浏览项目资产、预览模型/纹理、搜索过滤，并集成导入/导出工具。

- 其他面板：Hierarchy（层级视图，用于管理场景对象树）、Console（日志和渲染统计，如 FPS/DrawCalls）。

- 模式切换：Editor/Runtime 切换，支持 Play/Pause/Step，Runtime 时暂停编辑并运行游戏逻辑。


编辑器方案（与现有引擎结构对齐）

一、目标与范围
- 以当前引擎的 Layer + ImGui 架构为基础，提供可用的 Editor 模式：层级（Hierarchy）、检视（Inspector/Properties）、视口（Viewport）、控制台（Console/RenderInfo）等面板，以及基础的选取与操作（平移/旋转/缩放 Gizmos）。
- 初期不引入 ECS 与完整 Runtime，默认进入 Editor 状态并暂停游戏循环，后续逐步完善 Runtime/Play 模式。

二、与现有架构的契合点
- 主循环时序：每帧依次执行各 Layer 的 OnUpdate，然后由 ImGui 框架包裹 UI 绘制：ImGuiLayer::Begin() -> 各 Layer::OnImGuiRender() -> ImGuiLayer::End()。该模式已在 ImGui 层实现，Begin 内部创建 DockSpace 并管理布局，OnImGuiRender 暴露面板绘制入口。
  参考：<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="ImGuiLayer.h" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.h"></mcfile>
- 事件系统：平台端（GLFW）回调注入到引擎事件队列，主循环统一分发到各 Layer 的 OnEvent。ImGuiLayer 内部已使用 EventDispatcher 分发鼠标/键盘/窗口事件，且可通过 ImGuiIO 的 WantCaptureMouse/WantCaptureKeyboard 控制事件“吃掉/透传”。
  参考：<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="事件系统.md" path="e:\myGitRepos\GameEngine\文档\事件系统.md"></mcfile>
- 输入查询：在编辑器相机与快捷键中可复用 WindowsInput 抽象（基于 glfwGetKey / glfwGetMouseButton / glfwGetCursorPos）。
  参考：<mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile>

三、Layer 组织与插入点
- 新增 EditorLayer（普通 Layer，不是 Overlay），职责：
  1) 渲染编辑器各面板（在 OnImGuiRender 内）；
  2) 处理与编辑器相关的事件与状态（在 OnEvent/OnUpdate 内）；
  3) 管理编辑态数据：当前场景对象列表、当前选中对象、Gizmo 状态、相机控制等。
- LayerStack 建议顺序（示例）：
  1) RendererLayer（负责场景渲染/离屏帧缓冲）
  2) EditorLayer（绘制 UI 面板、转发/消费事件、操作场景数据）
  3) ImGuiLayer（Overlay：只负责 Begin/End 与 DockSpace/多视口管理）
- 数据流：RendererLayer 专注“如何渲染”，EditorLayer 负责“渲染什么 + 如何被用户操作”。EditorLayer 在 OnImGuiRender 中从 RendererLayer 拉取运行时统计/渲染目标纹理等进行展示；需要修改场景时，通过共享的 Scene/对象接口写回。

四、Docking 与面板布局
- DockSpace Host：保持在 ImGuiLayer::Begin 中创建与维护（无需将面板内容写在 ImGuiLayer 内）。
- 初始布局建议：
  - 左侧：Hierarchy（树状结构，列出场景中的对象）
  - 右侧：Inspector/Properties（显示并可编辑 Transform、材质等）
  - 底部：Console/RenderInfo（日志、帧率/DrawCall/内存等统计）
  - 中央：Viewport（渲染目标显示区，支持缩放/适配）
- 窗口命名规范："Hierarchy"、"Inspector"、"Viewport"、"Console"、"RenderInfo"（与 Begin/End 标签严格一致，便于 DockBuilder 记忆布局）。

五、Viewport 与渲染路径
- 离屏渲染：RendererLayer 将场景渲染到帧缓冲（Color+Depth，建议支持 MSAA）；EditorLayer 在 Viewport 面板中用 ImGui::Image 展示 Color 纹理。
- 视口尺寸变化：当 Viewport 面板大小变化时，重建/调整帧缓冲尺寸；维护“渲染分辨率”和“面板显示分辨率”的映射与 Letterbox 逻辑，确保不拉伸。
- 坐标换算：将 ImGui 面板内鼠标坐标转换为帧缓冲像素坐标，用于拾取/拖拽。

六、对象选取（Picking）与 Gizmos
- 选取方案（优先）：对象 ID 缓冲
  1) RendererLayer 额外输出一个 R32/UI 格式的“ObjectId”颜色附件；
  2) 鼠标点击时在该附件读回像素 ID 映射到对象；
  3) 支持多实例时，ObjectId 需在 DrawCall 级别区分（实例化可编码 InstanceId）。
- 备选：屏幕空间射线拾取（Ray Cast）
  1) 用相机投影将屏幕坐标反投影成世界射线；
  2) 与包围体/网格做相交测试，取最近命中。
- Gizmos（平移/旋转/缩放）
  - 初期：内置简化版 Gizmo（使用 ImDrawList 画三轴/圆环/方块手柄，命中测试在屏幕空间进行）。
  - 后续：可引入 ImGuizmo 以加速开发（当前仓库未集成，接口需留扩展点）。
  - 模式与坐标系：W/E/R 切换 Translate/Rotate/Scale；本地/世界坐标系切换；Snap 吸附（如 1/5/10 单位 或 5/15 度）。
  - 操作写回：Gizmo 计算出的变换回写至对象 Transform，并在 Inspector 面板即时同步显示。

七、编辑器相机与输入路由
- 相机控制（示例默认）：
  - 右键拖拽 + WASD：自由飞行（FPS）
  - 中键拖拽：平移（Pan）
  - 滚轮：缩放/前后推
  - F：聚焦到选中对象（Frame Selected）
- 输入路由规则：
  - 若 ImGui::GetIO().WantCaptureMouse 为真，鼠标事件不进入 3D 视口与相机；
  - 若鼠标在 Viewport 区域内且未被 UI 捕获，则事件交给 EditorLayer 的视口交互/相机控制；
  - 键盘同理参考 WantCaptureKeyboard；全局快捷键（如 Ctrl+S）可在未聚焦文本输入时生效。
- 复用输入封装：通过 <mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile> 查询键鼠状态；在 EditorLayer::OnEvent 与 OnUpdate 中综合使用（事件驱动 + 轮询）。

八、Inspector（Properties）与材质编辑
- Transform：位置/旋转/缩放（支持度数/弧度切换、重置按钮、归一化/合法化角度）。
- 材质：基础参数（颜色、金属度、粗糙度、纹理贴图选择/预览等），变更后通知 RendererLayer 刷新绑定。
- 组件扩展点：当前无 ECS，可先以“可选模块”的方式放在 Inspector 中（如：相机组件、灯光组件、脚本占位）。

九、Hierarchy 与场景管理
- 数据结构：临时 Scene/Node 结构维护父子关系与可见性；后续可平滑迁移到 ECS。
- 操作：
  - 创建/删除/重命名/拖拽重排（改变父子关系）；
  - 从文件路径导入模型（集成到现有资源载入流程，导入后将根节点插入场景）；
  - 选中同步：单击选中、Ctrl/Shift 多选（未来），与 Inspector/Viewport 同步。

十、Runtime/Editor 模式切换
- Toolbar 按钮：Play / Pause / Step / Stop。
- 切换策略：
  - 进入 Play：冻结 Editor 操作（或打上只读标记）、隐藏 Gizmo；启用脚本/物理更新；
  - 退出 Play：回到编辑前的场景状态（建议在进入 Play 前做一份运行时快照，避免编辑数据被运行时修改污染）。

十一、层间通信与依赖边界
- RendererLayer 不依赖 ImGui/Editor；EditorLayer 通过接口“拉取”统计数据/帧缓冲句柄（纹理 ID），以及“推送”场景变更（新增/删除对象、材质参数修改）。
- ImGuiLayer 不承载具体业务面板，仅提供 Begin/End 框架与 DockSpace。

十二、快捷键约定（建议）
- W/E/R：平移/旋转/缩放
- Q：无 Gizmo（选择模式）
- F：聚焦选中对象
- Delete：删除选中对象
- Ctrl+D：复制选中对象
- Ctrl+S：保存场景（占位）

十三、保存/加载与状态持久化（阶段性 TODO）
- 场景保存格式：JSON/自定义二进制（阶段 1 建议 JSON，便于调试）。
- 内容：对象层级、Transform、材质/网格引用、相机/灯光、环境设置。
- 资源路径：统一相对工程根路径，避免绝对路径依赖。

十四、实现步骤（里程碑）
- M1：EditorLayer 搭建
  1) 新建 EditorLayer 并加入 LayerStack；
  2) 在 OnImGuiRender 中实现 DockSpace 下的四个基础面板（Hierarchy/Inspector/Viewport/Console）；
  3) RendererLayer 输出离屏帧缓冲，Viewport 显示并随面板尺寸自适应。
- M2：交互与选取
  1) Viewport 鼠标坐标变换与边界判断；
  2) 射线拾取或 ID 缓冲选取（优先 ID 缓冲）；
  3) 选中高亮与 Inspector 同步；
  4) 编辑器相机控制与输入路由完善。
- M3：Gizmos 与变换写回
  1) 简化版三轴/旋转环/缩放手柄；
  2) 支持世界/本地、Snap；
  3) 与 Transform/渲染同步。
- M4：资产导入与层级操作
  1) 从文件路径导入模型并插入场景；
  2) Hierarchy 拖拽重排、删除/复制；
  3) Console/RenderInfo 填充运行时统计（来自 RendererLayer）。
- M5：Runtime/Editor 切换雏形
  1) Toolbar 与状态机；
  2) 进入/退出 Play 的场景快照；
  3) 基础脚本更新开关。

十五、注意事项
- 多视口（ViewportsEnable）与 Docking 已在 ImGui 层初始化，保持一致；
- 事件“吞吐”遵循 ImGuiIO 标志，避免 UI 与 3D 交互相互干扰；
- 坐标系与单位统一（右手/左手、度/弧、缩放单位）；
- 帧缓冲重建与资源生命周期管理（避免频繁创建销毁导致抖动）；
- 性能基准与调试：在 Console/RenderInfo 暴露 FPS、CPU/GPU 时间、DrawCalls、三角形数等指标。

参考文件
- ImGui 框架与布局：<mcfile name="ImGuiLayer.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.cpp"></mcfile> <mcfile name="ImGuiLayer.h" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\Imgui\ImGuiLayer.h"></mcfile>
- 事件系统与输入：<mcfile name="事件系统.md" path="e:\myGitRepos\GameEngine\文档\事件系统.md"></mcfile> <mcfile name="WindowsInput.cpp" path="e:\myGitRepos\GameEngine\GameEngine\src\Engine\platform\WindowsInput.cpp"></mcfile>