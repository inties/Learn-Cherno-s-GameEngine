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
namespace fs = std::filesystem;
namespace Engine {
	// ���岢������̬��Ա���� 

	ENGINE_API Application* Application::s_Instance = nullptr;
	Application::Application() {
		std::cout << "��ǰ����Ŀ¼: " << fs::current_path() << std::endl;
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
		
		// ��˳����Ӳ㣺����ImGui�㡢����Ⱦ�㣬Ȼ���ǲ��Բ�
		m_ImGuiLayer = new ImguiLayer();
		m_LayerStack.PushLayer(m_ImGuiLayer);
		m_LayerStack.PushLayer(new RendererLayer());
		m_LayerStack.PushLayer(new EditorLayer());
		//m_LayerStack.PushLayer(new TestLayer());
		
		// ��ʼ��Camera����
		Camera::Initialize(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
		
		// ��ʼ��֡��ʱ
		m_LastFrameTime = glfwGetTime();
		
		// ע�⣺Renderer::Init() ������ RendererLayer::OnAttach() �е���
		
		// Initialization code here
	}
	
	Application::~Application() {
		// Cleanup code here
	}
	
	void Application::run() {
		while (m_Running) {
			// ����֡��ʱ
			UpdateFrameTiming();
			
			// �����������������
			ProcessCameraInput();
			
			// �Ƴ��ֶ����������룬������ RendererLayer ����
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnUpdate();
			}
			
			// �ڿ�ʼ ImGui ֮֡ǰ������Ĭ��֡���壬���� UI �϶�������Ӱ
			RenderCommand::SetClearColor(glm::vec4(0.10f, 0.10f, 0.10f, 1.0f));
			RenderCommand::Clear();
			
			m_ImGuiLayer->Begin();
			for (auto layer : m_LayerStack.m_Layers) {
				layer->OnImGuiRender();
			}
			// ��ʾ�����������Ϣ
			
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
		
		// WASD������������ƶ�
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
		if (dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return this->OnWindowClose(e); }))return true;
		
		// ���ڴ��ڴ�С�����¼�������Application�㴦��Ȼ�󴫲������в�
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { 
			this->OnWindowResize(e); 
			return false; // �����أ����¼���������
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
	
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
	
	void Application::OnWindowResize(WindowResizeEvent& e) {
		ENGINE_CORE_INFO("Application: Window Resized to {}x{}", e.GetWindowWidth(), e.GetWindowHeight());

		// ������Ⱦ���ӿڣ�ȫ�ֲ���Ĵ���
		Renderer::OnWindowResize(e.GetWindowWidth(), e.GetWindowHeight());
		
		// ע�⣺����������ڱ����Ϊvoid����Ϊ�¼�������OnEvent��������
	}
}