#include "pch.h"
#include "SandBox.h"


SandBox::SandBox()
{
	// ��Ӳ��Բ�
}

//
SandBox::~SandBox()
{
   
}

// ������ڵ�
Engine::Application* Engine::createApplication() 
{
	return new SandBox();
}
