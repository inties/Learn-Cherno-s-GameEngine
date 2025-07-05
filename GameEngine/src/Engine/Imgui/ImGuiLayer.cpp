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

// �����¼��󶨺�
#define BIND_CLASS_FUC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {

	ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
	{
		// ���캯��������ȷ���û��๹�캯��
	}

	void ImguiLayer::OnAttach()
	{
		// ����ImGui������
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// ����ImGui IO����
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		// �Ƴ�����KeyMap���� - ���°汾ImGui���ѱ�����
		// �ִ�ImGui�汾�ĺ�˻��Զ��������ӳ��
		Application& app = Application::Get();
		Windows& window = app.GetWindow();
		WindowWindows& windowWindows = dynamic_cast<WindowWindows&>(window);
		// ��ȡGLFW����ָ��
		GLFWwindow* glwindow = windowWindows.GetGLWindow();

		// ��ʼ��ImGui��GLFW��OpenGL3���
		ImGui_ImplGlfw_InitForOpenGL(glwindow, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImguiLayer::OnDetach()
	{
		// ����ImGui��Դ
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
		// ������ʾ�ߴ��ʱ��
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		// ��ʼ�µ�ImGui֡
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ��ȾUI
		static bool show = true;
		 ImGui::ShowDemoWindow(&show);  // ���������������ʾImGui��ʾ����

		// �Զ���UI
		//MyInteractiveUI();

		// ��ȾImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImguiLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		
		// �ַ������¼�����
		dispatcher.Dispatch<MouseMoveEvent>(BIND_CLASS_FUC(OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_CLASS_FUC(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_CLASS_FUC(OnMouseButtonReleased));
		dispatcher.Dispatch<KeyPressEvent>(BIND_CLASS_FUC(OnKeyPressed));
		dispatcher.Dispatch<KeyReleaseEvent>(BIND_CLASS_FUC(OnKeyReleased));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_CLASS_FUC(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_CLASS_FUC(OnWindowResize));
	}

	// ����¼�����
	bool ImguiLayer::OnMouseMove(MouseMoveEvent& e)
	{
		// ��������ƶ��¼�
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2((float)e.getMousePos().first, (float)e.getMousePos().second);
		
		// ���ImGui��Ҫ������꣬����ֹ�¼���������
		return m_BlockEvents && io.WantCaptureMouse;
	}

	bool ImguiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int button = e.getMouseButton();
		
		// ��GLFW��갴ťӳ�䵽ImGui
		if (button >= 0 && button < ImGuiMouseButton_COUNT) {
			io.MouseDown[button] = true;
		}
		
		// �������
		std::cout << "Mouse button pressed: " << button << std::endl;
		
		// ���ImGui��Ҫ������꣬����ֹ�¼���������
		return m_BlockEvents && io.WantCaptureMouse;
	}

	bool ImguiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int button = e.getMouseButton();
		
		// ��GLFW��갴ťӳ�䵽ImGui
		if (button >= 0 && button < ImGuiMouseButton_COUNT) {
			io.MouseDown[button] = false;
		}
		
		// �������
		std::cout << "Mouse button released: " << button << std::endl;
		
		// ���ImGui��Ҫ������꣬����ֹ�¼���������
		return m_BlockEvents && io.WantCaptureMouse;
	}

	// �����¼�����
	bool ImguiLayer::OnKeyPressed(KeyPressEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		// ���������
		if (e.ToString().find("KeyPressed: 286") != std::string::npos) { // F5��
			m_ShowDemoWindow = !m_ShowDemoWindow;
			std::cout << "Demo window toggled: " << (m_ShowDemoWindow ? "ON" : "OFF") << std::endl;
		}
		
		// �������
		std::cout << e.ToString() << std::endl;
		
		// ���ImGui��Ҫ������̣�����ֹ�¼���������
		return m_BlockEvents && io.WantCaptureKeyboard;
	}

	bool ImguiLayer::OnKeyReleased(KeyReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		// �������
		std::cout << e.ToString() << std::endl;
		
		// ���ImGui��Ҫ������̣�����ֹ�¼���������
		return m_BlockEvents && io.WantCaptureKeyboard;
	}

	// �����¼�����
	bool ImguiLayer::OnWindowClose(WindowCloseEvent& e)
	{
		std::cout << "Window close event received" << std::endl;
		
		// ���ڹر��¼�ͨ������Ҫ��ImGui��ֹ
		// ������������ӱ���ImGui���ֵ��߼�
		return false;
	}

	bool ImguiLayer::OnWindowResize(WindowResizeEvent& e)
	{
		std::cout << "Window resize event: " << e.GetWindowWidth() << "x" << e.GetWindowHeight() << std::endl;
		
		// ����ImGui����ʾ�ߴ�
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)e.GetWindowWidth(), (float)e.GetWindowHeight());
		
		// ���ڴ�С�ı��¼�ͨ������Ҫ��ImGui��ֹ
		return false;
	}
}