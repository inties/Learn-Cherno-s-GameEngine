#include "SandBox.h"
#include<Engine.h>
#include <iostream>
#include <glad/glad.h>
#include<GLFW/glfw3.h>
SandBox::SandBox()
{
	Engine::Windows::WindowsProps props = { "SandBox", 800, 600 };
	m_Window = std::unique_ptr<Engine::Windows>(Engine::Windows::Create(props));
	if (m_Window == nullptr)
	{
		std::cout << "Window creation failed!" << std::endl;
		return;
	}
	
	m_Window->SetEventCallBack([this](Engine::Event& e) {
		return this->OnEvent(e);
	});
}
//
SandBox::~SandBox()
{
   
}
void SandBox::run() {
	while (m_Running) {
		m_Window->Update();
	}
}
// 引擎入口点
Engine::Application* Engine::createApplication() 
{
    return new SandBox();
}
