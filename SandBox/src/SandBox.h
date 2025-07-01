#pragma once

#include <Engine.h>
#include<memory>
class SandBox : public Engine::Application
{
public:
	SandBox();
	virtual ~SandBox();
	
	void run() override;
	Engine::Windows& GetWindow() override { return *m_Window; }
	
private:
	std::unique_ptr<Engine::Windows> m_Window; // ���ڶ���
};
