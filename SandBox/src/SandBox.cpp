#include "SandBox.h"
#include<Engine.h>
#include <iostream>
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<Engine/Imgui/ImGuiLayer.h>
SandBox::SandBox()
{
	Engine::Windows::WindowsProps props = { "SandBox", 1800, 1800 };
	m_Window = std::unique_ptr<Engine::Windows>(Engine::Windows::Create(props));
	if (m_Window == nullptr)
	{
		std::cout << "Window creation failed!" << std::endl;
		return;
	}
	
	m_Window->SetEventCallBack([this](Engine::Event& e) {
		return this->OnEvent(e);
	});
	m_LayerStack.PushLayer(new Engine::ImguiLayer());
}
//
SandBox::~SandBox()
{
   
}
void SandBox::run() {
	while (m_Running) {
	
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_LayerStack.IterateLayers();
		m_Window->Update();
	}
}
// 引擎入口点
Engine::Application* Engine::createApplication() 
{
    return new SandBox();
}
