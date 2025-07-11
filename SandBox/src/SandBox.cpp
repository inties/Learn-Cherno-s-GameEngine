#include "SandBox.h"
#include<Engine.h>
#include <iostream>
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<Engine/Imgui/ImGuiLayer.h>

SandBox::SandBox()
{
	



}
//
SandBox::~SandBox()
{
   
}

// 引擎入口点
Engine::Application* Engine::createApplication() 
{
	return new SandBox();
}
