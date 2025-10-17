#include"pch.h"
#include "Application.h"
#include "log.h"
#include"Imgui/ImGuiLayer.h"
#include "RendererLayer.h"
#include "TestLayer.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include "Editor/EditorLayer.h"
#include "Engine/platform/WindowWindows.h"
namespace fs = std::filesystem;
namespace Engine {
	// 定义并导出静态成员变量 

	ENGINE_API Application* Application::s_Instance = nullptr;
	Application::Application() {
		std::cout << fs::current_path() << std::endl;
		s_Instance = this;
		m_Running = true;
		Windows::WindowsProps props = { "Application", 1800, 1800 };
		m_Window = std::unique_ptr<Engine::Windows>(Engine::Windows::Create(props));
		if (m_Window == nullptr)
		{
			std::cout << "Window creation failed!" << std::endl;
			return;
		}

		m_Window->SetEventCallBack([this](Engine::Event& e) {
			return this->OnEvent(e);
			});
		
		// 按顺序添加层：首先ImGui层、是渲染层，然后是测试层

		//各层分别初始化与其他层无关的成员变量
		m_ImGuiLayer = new ImguiLayer();
		RendererLayer* renderlayer = new RendererLayer();
		if (!renderlayer) {
			std::cout << "空" << " ";
		}
		EditorLayer* editorlayer = new EditorLayer();

		//注入各层所需依赖
		renderlayer->Init(editorlayer->GetScene());
		editorlayer->Init(renderlayer);
		m_LayerStack.PushLayer(m_ImGuiLayer);
		m_LayerStack.PushLayer(renderlayer);
		m_LayerStack.PushLayer(editorlayer);
		//m_LayerStack.PushLayer(new TestLayer());
		
		// 初始化Camera单例
		Camera::Initialize(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
		


		
		// 注意：Renderer::Init() 现在在 RendererLayer::OnAttach() 中调用
		
		// Initialization code here
	}
	
	Application::~Application() {
		// Cleanup code here
	}
	
	void Application::run() {
		while (m_Running) {
			// 更新帧计时
			UpdateFrameTiming();
			
			// 处理摄像机键盘输入
			ProcessCameraInput();
			
			// 移除手动的清屏代码，现在由 RendererLayer 处理
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnUpdate();
			}
			
			// 在开始 ImGui 帧之前，清理默认帧缓冲，避免 UI 拖动产生残影
			RenderCommand::SetClearColor(glm::vec4(0.10f, 0.10f, 0.10f, 1.0f));
			RenderCommand::Clear();
			
			m_ImGuiLayer->Begin();
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnImGuiRender();
			}
			// 显示摄像机调试信息
			
			m_ImGuiLayer->End();
			
			m_Window->Update();
		}
	}
	
	void Application::UpdateFrameTiming() {
		float currentFrameTime = glfwGetTime();
		m_DeltaTime = currentFrameTime - m_LastFrameTime;
		m_LastFrameTime = currentFrameTime;
	}
	
	void Application::ProcessCameraInput() {
		auto input = Input::GetInstance();
		Camera* camera = Camera::GetInstance();
		if (!camera) return;
		
		// WASD键控制摄像机移动
		if (input->IsKeyPressed(EG_KEY_W)) {
			camera->ProcessKeyboard(Camera_Movement::FORWARD, m_DeltaTime);
		}
		if (input->IsKeyPressed(EG_KEY_S)) {
			camera->ProcessKeyboard(Camera_Movement::BACKWARD, m_DeltaTime);
		}
		if (input->IsKeyPressed(EG_KEY_A)) {
			camera->ProcessKeyboard(Camera_Movement::LEFT, m_DeltaTime);
		}
		if (input->IsKeyPressed(EG_KEY_D)) {
			camera->ProcessKeyboard(Camera_Movement::RIGHT, m_DeltaTime);
		}
	}
	
	
	
	bool Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		if (dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent& e) { return this->OnMouseMove(e); }))return true;
		if (dispatcher.Dispatch<MouseScrollEvent>([this](MouseScrollEvent& e) { return this->OnMouseScroll(e); }))return true;
		if (dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent& e) { return this->OnKeyPress(e); }))return true;
		if (dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return this->OnWindowClose(e); }))return true;
		
		// 对于窗口大小调整事件，先在Application层处理，然后传播给所有层
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { 
			this->OnWindowResize(e); 
			return false; // 不拦截，让事件继续传播
		});

		if (m_LayerStack.ProcessEvents(e))return true;
		/*if(dispatcher.Dispatch(OnKey(e)))return true;*/
		return false;
	}
	
	bool Application::OnMouseMove(MouseMoveEvent& e) {
		// ENGINE_CORE_INFO("Mouse moved to {0},{1}", e.getMousePos().first, e.getMousePos().second);
		auto [x, y] = e.getMousePos();

		Camera* camera = Camera::GetInstance();
		if (camera) {
			camera->ProcessMouseMovement(x, y);
		}
		return true;
	}

	bool Application::OnKeyPress(KeyPressEvent& e) {
		// ESC键切换鼠标锁定模式
		if (e.GetKeyCode() == EG_KEY_ESCAPE) {
			static bool cursorDisabled = false;
			cursorDisabled = !cursorDisabled;
			
			// 获取窗口实例并设置光标模式
			if (auto* windowsWindow = dynamic_cast<WindowWindows*>(m_Window.get())) {
				windowsWindow->SetCursorMode(cursorDisabled);
			}
			return true;
		}
		return false;
	}
	
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
	
	void Application::OnWindowResize(WindowResizeEvent& e) {
		ENGINE_CORE_INFO("Application: Window Resized to {}x{}", e.GetWindowWidth(), e.GetWindowHeight());

		//// 更新渲染器视口（全局层面的处理）
		//Renderer::OnWindowResize(e.GetWindowWidth(), e.GetWindowHeight());
		
		// 注意：这个方法现在被设计为void，因为事件传播由OnEvent方法控制
	}

	bool Application::OnMouseScroll(MouseScrollEvent& e) {
		Camera* camera = Camera::GetInstance();
		if (camera) {
			camera->ProcessMouseScroll(e.getYOffset());
		}
		return true;
	}
}
