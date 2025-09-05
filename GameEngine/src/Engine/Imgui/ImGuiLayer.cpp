#include "pch.h"  
#include "Engine/core.h"
#include "ImGuiLayer.h"  

#include "Engine/Application.h"
#include "Engine/Windows.h"
#include "Engine/platform/WindowWindows.h"
#include "imgui.h"
#include "Engine/Imgui/ImguiOpenglRender.h"
#include "Engine/Imgui/imgui_impl_glfw.h"
#include "Engine/Imgui/imgui_internal.h"
#include <GLFW/glfw3.h>

// 定义事件绑定宏
#define BIND_CLASS_FUC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {

	ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
	{
		// 构造函数现在正确调用基类构造函数
	}
	ImguiLayer::~ImguiLayer(){

	}

	void ImguiLayer::OnAttach()
	{
		// 创建ImGui上下文
		
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// 禁用 ImGui 自动从磁盘加载/保存 ini（我们自行提供保存/加载入口）
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	
		// 移除所有KeyMap设置 - 在新版本ImGui中已被废弃
		// 现代ImGui版本的后端会自动处理键盘映射
		Application& app = Application::Get();
		Windows& window = app.GetWindow();
		WindowWindows& windowWindows = dynamic_cast<WindowWindows&>(window);
		// 获取GLFW窗口指针
		GLFWwindow* glwindow = windowWindows.GetGLWindow();

		// 初始化ImGui的GLFW和OpenGL3后端
		ImGui_ImplGlfw_InitForOpenGL(glwindow, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImguiLayer::OnDetach()
	{
		// 清理ImGui资源
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void MyInteractiveUI() {
		ImGui::Begin("Hello, ImGui!");
		ImGui::Text("demo");
		if (ImGui::Button("press me!")) {
			ImGui::Text("clicked!");
			std::cout << "Button clicked!" << std::endl;
		}
		ImGui::End();
	}
	void ImguiLayer::Begin(){
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void ImguiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void ImguiLayer::OnImGuiRender()
	{
		/*static bool show = true;
		ImGui::ShowDemoWindow(&show);*/
		// --- 步骤 1: 准备一个全屏的宿主窗口，它将作为所有停靠窗口的“底座” ---

		static bool opt_fullscreen = true; // 保持全屏
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		// 我们使用 ImGuiWindowFlags_NoDocking 标志来使父窗口本身不可停靠，
		// 因为在一个停靠目标内部再嵌套一个停靠目标会令人困惑。
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// 当使用 ImGuiDockNodeFlags_PassthruCentralNode 标志时，DockSpace 会渲染背景，
		// 所以我们要求 Begin() 不要渲染背景，以实现透明效果。
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// **关键：让宿主窗口透明**
		// 我们直接设置一个透明的背景，这样就不会有双层背景色的问题。
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0)); // RGBA(0,0,0,0) -> 完全透明
		ImGui::Begin("DockSpace Host", nullptr, window_flags);
		ImGui::PopStyleColor(); // 弹出颜色设置，以免影响其他窗口

		if (opt_fullscreen)
			ImGui::PopStyleVar(2); // 弹出之前设置的 rounding 和 border size

		// --- 顶部菜单：提供 保存/加载/重置 布局 ---
		static const char* kUserLayoutPath = "SandBox/imgui_user_layout.ini";
		static bool s_buildDefaultLayout = true; // 程序启动时使用默认布局；用户可通过“重置”再次触发
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Layout"))
			{
				if (ImGui::MenuItem("Save Layout"))
				{
					ImGui::SaveIniSettingsToDisk(kUserLayoutPath);
				}
				if (ImGui::MenuItem("Load Layout"))
				{
					ImGui::LoadIniSettingsFromDisk(kUserLayoutPath);
					// 加载自定义布局后，不再强制构建默认布局
					s_buildDefaultLayout = false;
				}
				if (ImGui::MenuItem("Reset to Default"))
				{
					// 下一个阶段使用默认布局
					s_buildDefaultLayout = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// --- 步骤 2: 提交 DockSpace 并设置一次性默认布局 ---

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			// 使用 DockBuilder API 设置默认布局（仅当 s_buildDefaultLayout 为 true 时触发一次）
			if (s_buildDefaultLayout)
			{
				// 构建完成后立即关闭开关，避免每帧重建
				s_buildDefaultLayout = false;

				// 清除并重建根节点（不重复创建宿主窗口，仅重建 DockBuilder 结构）
				ImGui::DockBuilderRemoveNode(dockspace_id);
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->WorkSize);

				// 将主停靠区分割成：左 20%、右 25%、下 25%、中间剩余
				ImGuiID dock_main_id = dockspace_id;
				ImGuiID dock_id_left  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left,  0.20f, nullptr, &dock_main_id);
				ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
				ImGuiID dock_id_down  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down,  0.25f, nullptr, &dock_main_id);

				// 按约定的窗口名进行 Dock：这些窗口由 EditorLayer/RendererLayer 创建
				ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
				ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
				ImGui::DockBuilderDockWindow("Console",   dock_id_down);
				ImGui::DockBuilderDockWindow("Viewport",  dock_main_id); // 中央区域
				ImGui::DockBuilderDockWindow("RenderInfo", dock_id_right); // 与 Inspector 同列（Tab）

				// 完成布局设置
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}
		else
		{
			// 显示提示信息，如果 docking 未启用
			ImGui::Text("ERROR: Docking is not enabled! Please add 'io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;' to your code.");
		}

		// 结束宿主窗口
		ImGui::End();


		// --- 步骤 3: 渲染你的可停靠窗口 ---
		// 这些窗口由 EditorLayer/RendererLayer 各自负责创建与渲染，这里无需重复创建占位窗口。
		// ImGui 会根据 DockBuilder 设置的布局，自动将它们绘制在正确的位置。
	}
	void ImguiLayer::ImGuiRender()
	{
		/*static bool show = true;
		ImGui::ShowDemoWindow(&show);*/
		
	}
	void ImguiLayer::OnUpdate()
	{
		
	}

	void ImguiLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		
		// 分发各种事件类型
		dispatcher.Dispatch<MouseMoveEvent>(BIND_CLASS_FUC(OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_CLASS_FUC(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_CLASS_FUC(OnMouseButtonReleased));
		dispatcher.Dispatch<KeyPressEvent>(BIND_CLASS_FUC(OnKeyPressed));
		dispatcher.Dispatch<KeyReleaseEvent>(BIND_CLASS_FUC(OnKeyReleased));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_CLASS_FUC(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_CLASS_FUC(OnWindowResize));
	}

	// 鼠标事件处理
	bool ImguiLayer::OnMouseMove(MouseMoveEvent& e)
	{
		// 处理鼠标移动事件
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2((float)e.getMousePos().first, (float)e.getMousePos().second);
		
		// 如果ImGui想要捕获鼠标，则阻止事件继续传播
		return m_BlockEvents&&io.WantCaptureMouse;
	}

	bool ImguiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int button = e.getMouseButton();
		
		// 将GLFW鼠标按钮映射到ImGui
		if (button >= 0 && button < ImGuiMouseButton_COUNT) {
			io.MouseDown[button] = true;
		}
		
		// 调试输出
		// std::cout << "Mouse button pressed: " << button << std::endl;
		
		// 如果ImGui想要捕获鼠标，则阻止事件继续传播
		return m_BlockEvents && io.WantCaptureMouse;
	}

	bool ImguiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int button = e.getMouseButton();
		
		// 将GLFW鼠标按钮映射到ImGui
		if (button >= 0 && button < ImGuiMouseButton_COUNT) {
			io.MouseDown[button] = false;
		}
		
		// 调试输出
		// std::cout << "Mouse button released: " << button << std::endl;
		
		// 如果ImGui想要捕获鼠标，则阻止事件继续传播
		return m_BlockEvents && io.WantCaptureMouse;
	}

	// 键盘事件处理
	bool ImguiLayer::OnKeyPressed(KeyPressEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		// 特殊键处理
		if (e.ToString().find("KeyPressed: 286") != std::string::npos) { // F5键
			m_ShowDemoWindow = !m_ShowDemoWindow;
			std::cout << "Demo window toggled: " << (m_ShowDemoWindow ? "ON" : "OFF") << std::endl;
		}
		
		// 调试输出
		std::cout << e.ToString() << std::endl;
		
		// 如果ImGui想要捕获键盘，则阻止事件继续传播
		return m_BlockEvents && io.WantCaptureKeyboard;
	}

	bool ImguiLayer::OnKeyReleased(KeyReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		// 调试输出
		std::cout << e.ToString() << std::endl;
		
		// 如果ImGui想要捕获键盘，则阻止事件继续传播
		return m_BlockEvents && io.WantCaptureKeyboard;
	}

	// 窗口事件处理
	bool ImguiLayer::OnWindowClose(WindowCloseEvent& e)
	{
		std::cout << "Window close event received" << std::endl;
		
		// 窗口关闭事件通常不需要被ImGui阻止
		// 可以在这里添加保存ImGui布局等逻辑
		return false;
	}

	bool ImguiLayer::OnWindowResize(WindowResizeEvent& e)
	{
		std::cout << "Window resize event: " << e.GetWindowWidth() << "x" << e.GetWindowHeight() << std::endl;
		
		// 更新ImGui的显示尺寸
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)e.GetWindowWidth(), (float)e.GetWindowHeight());
		
		// 窗口大小改变事件通常不需要被ImGui阻止
		return false;
	}
}