#include"pch.h"
#include "Application.h"
#include"log.h"
#include"Imgui/ImGuiLayer.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h>
namespace Engine {
	Application* Application::s_Instance = nullptr;
	Application::Application() {
		s_Instance = this;
		m_Running = true;
		
		// Initialization code here
	}
	
	Application::~Application() {
		// Cleanup code here
	}
	
	void Application::run() {
		
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
		//ENGINE_CORE_INFO("Mouse moved to {0},{1}", e.getMousePos().first, e.getMousePos().second);
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