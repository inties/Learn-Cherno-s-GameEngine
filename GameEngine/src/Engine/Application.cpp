#include"pch.h"
#include "Application.h"
#include"log.h"
#include"Imgui/ImGuiLayer.h"
#include "TestLayer.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Renderer/Renderer.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h>
namespace Engine {
	// 定义并导出静态成员变量
	ENGINE_API Application* Application::s_Instance = nullptr;
	Application::Application() {
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
		m_ImGuiLayer = new ImguiLayer();
		m_LayerStack.PushLayer(m_ImGuiLayer);
		m_LayerStack.PushLayer(new TestLayer());
		
		// 初始化渲染器
		Renderer::Init();
		
		// Initialization code here
	}
	
	Application::~Application() {
		// Cleanup code here
	}
	
	void Application::run() {
		while (m_Running) {
			if (Input::GetInstance()->IsKeyPressed(EG_KEY_A)) {
				ENGINE_CORE_INFO("{0}has been pressed", (char)EG_KEY_A);
			}
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnUpdate();
			}
			m_ImGuiLayer->Begin();
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			m_Window->Update();
		
		}
	}
	bool Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		if (dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent& e) { return this->OnMouseMove(e); }))return true;
		if (dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return this->OnWindowClose(e); }))return true;


		if (m_LayerStack.ProcessEvents(e))return true;
		/*if(dispatcher.Dispatch(OnKey(e)))return true;*/
		return false;
	}
	bool Application::OnMouseMove(MouseMoveEvent& e) {
		// ENGINE_CORE_INFO("Mouse moved to {0},{1}", e.getMousePos().first, e.getMousePos().second);
		return true;
	}
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e) {
		ENGINE_CORE_INFO("Window Resized");

		return true; 
		
	}
}