#include "pch.h"
#include "SandBox.h"


SandBox::SandBox()
{
	// 添加测试层
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
