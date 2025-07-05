#include "pch.h"  
#include "Engine/core.h"
#include "ImGuiLayer.h"  

#include "Engine/Application.h"
#include "Engine/Windows.h"
#include "Engine/WindowWindows.h"
#include "imgui.h"
#include "Engine/platform/ImguiOpenglRender.h"
#include "Engine/platform/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

// 定义事件绑定宏
#define BIND_CLASS_FUC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {

	ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
	{
		// 构造函数现在正确调用基类构造函数
	}

	void ImguiLayer::OnAttach()
	{
		// 创建ImGui上下文
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// 设置ImGui IO配置
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

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

	void ImguiLayer::OnUpdate()
	{
		// 设置显示尺寸和时间
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		// 开始新的ImGui帧
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 渲染UI
		static bool show = true;
		 ImGui::ShowDemoWindow(&show);  // 可以启用这个来显示ImGui演示窗口

		// 自定义UI
		//MyInteractiveUI();

		// 渲染ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		return m_BlockEvents && io.WantCaptureMouse;
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
		std::cout << "Mouse button pressed: " << button << std::endl;
		
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
		std::cout << "Mouse button released: " << button << std::endl;
		
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