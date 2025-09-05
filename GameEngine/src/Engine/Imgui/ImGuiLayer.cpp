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

// �����¼��󶨺�
#define BIND_CLASS_FUC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {

	ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
	{
		// ���캯��������ȷ���û��๹�캯��
	}
	ImguiLayer::~ImguiLayer(){

	}

	void ImguiLayer::OnAttach()
	{
		// ����ImGui������
		
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// ���� ImGui �Զ��Ӵ��̼���/���� ini�����������ṩ����/������ڣ�
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
		// --- ���� 1: ׼��һ��ȫ�����������ڣ�������Ϊ����ͣ�����ڵġ������� ---

		static bool opt_fullscreen = true; // ����ȫ��
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		// ����ʹ�� ImGuiWindowFlags_NoDocking ��־��ʹ�����ڱ�����ͣ����
		// ��Ϊ��һ��ͣ��Ŀ���ڲ���Ƕ��һ��ͣ��Ŀ�����������
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

		// ��ʹ�� ImGuiDockNodeFlags_PassthruCentralNode ��־ʱ��DockSpace ����Ⱦ������
		// ��������Ҫ�� Begin() ��Ҫ��Ⱦ��������ʵ��͸��Ч����
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// **�ؼ�������������͸��**
		// ����ֱ������һ��͸���ı����������Ͳ�����˫�㱳��ɫ�����⡣
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0)); // RGBA(0,0,0,0) -> ��ȫ͸��
		ImGui::Begin("DockSpace Host", nullptr, window_flags);
		ImGui::PopStyleColor(); // ������ɫ���ã�����Ӱ����������

		if (opt_fullscreen)
			ImGui::PopStyleVar(2); // ����֮ǰ���õ� rounding �� border size

		// --- �����˵����ṩ ����/����/���� ���� ---
		static const char* kUserLayoutPath = "SandBox/imgui_user_layout.ini";
		static bool s_buildDefaultLayout = true; // ��������ʱʹ��Ĭ�ϲ��֣��û���ͨ�������á��ٴδ���
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
					// �����Զ��岼�ֺ󣬲���ǿ�ƹ���Ĭ�ϲ���
					s_buildDefaultLayout = false;
				}
				if (ImGui::MenuItem("Reset to Default"))
				{
					// ��һ���׶�ʹ��Ĭ�ϲ���
					s_buildDefaultLayout = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// --- ���� 2: �ύ DockSpace ������һ����Ĭ�ϲ��� ---

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			// ʹ�� DockBuilder API ����Ĭ�ϲ��֣����� s_buildDefaultLayout Ϊ true ʱ����һ�Σ�
			if (s_buildDefaultLayout)
			{
				// ������ɺ������رտ��أ�����ÿ֡�ؽ�
				s_buildDefaultLayout = false;

				// ������ؽ����ڵ㣨���ظ������������ڣ����ؽ� DockBuilder �ṹ��
				ImGui::DockBuilderRemoveNode(dockspace_id);
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->WorkSize);

				// ����ͣ�����ָ�ɣ��� 20%���� 25%���� 25%���м�ʣ��
				ImGuiID dock_main_id = dockspace_id;
				ImGuiID dock_id_left  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left,  0.20f, nullptr, &dock_main_id);
				ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
				ImGuiID dock_id_down  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down,  0.25f, nullptr, &dock_main_id);

				// ��Լ���Ĵ��������� Dock����Щ������ EditorLayer/RendererLayer ����
				ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
				ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
				ImGui::DockBuilderDockWindow("Console",   dock_id_down);
				ImGui::DockBuilderDockWindow("Viewport",  dock_main_id); // ��������
				ImGui::DockBuilderDockWindow("RenderInfo", dock_id_right); // �� Inspector ͬ�У�Tab��

				// ��ɲ�������
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}
		else
		{
			// ��ʾ��ʾ��Ϣ����� docking δ����
			ImGui::Text("ERROR: Docking is not enabled! Please add 'io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;' to your code.");
		}

		// ������������
		ImGui::End();


		// --- ���� 3: ��Ⱦ��Ŀ�ͣ������ ---
		// ��Щ������ EditorLayer/RendererLayer ���Ը��𴴽�����Ⱦ�����������ظ�����ռλ���ڡ�
		// ImGui ����� DockBuilder ���õĲ��֣��Զ������ǻ�������ȷ��λ�á�
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
		return m_BlockEvents&&io.WantCaptureMouse;
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
		// std::cout << "Mouse button pressed: " << button << std::endl;
		
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
		// std::cout << "Mouse button released: " << button << std::endl;
		
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