#pragma once
#include"pch.h"
#include "core.h"
#include"Event.h"
namespace Engine {
	class Windows; // Forward declaration
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void run();
		virtual bool OnEvent(Event& e);
		virtual bool OnMouseMove(MouseMoveEvent& e);
		virtual bool OnWindowClose(WindowCloseEvent& e);
		virtual bool OnWindowResize(WindowResizeEvent& e);
		//virtual bool OnKeyPress(KeyPressEvent& e);
		//virtual bool OnKeyRelease(KeyReleaseEvent& e);
		inline static Application& Get() { return *s_Instance; }
		virtual Windows& GetWindow() = 0;
	protected:
		bool m_Running = false;
		static Application* s_Instance;
	};
	
	// 由客户端实现
	Application* createApplication();

}

