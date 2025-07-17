#pragma once
#include"pch.h"
#include "core.h"
#include"Event.h"
#include"LayerStack.h"
#include"Windows.h"
#include "camera.h"
#include "Imgui/ImGuiLayer.h"

namespace Engine {
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void run();
		virtual bool OnEvent(Event& e);
		virtual bool OnMouseMove(MouseMoveEvent& e);
		virtual bool OnWindowClose(WindowCloseEvent& e);
		virtual void OnWindowResize(WindowResizeEvent& e);
		//virtual bool OnKeyPress(KeyPressEvent& e);
		//virtual bool OnKeyRelease(KeyReleaseEvent& e);
		
		// 摄像机控制相关方法
		void ProcessCameraInput();
		void UpdateFrameTiming();
		
		std::unique_ptr<Camera>mainCam;
		inline static Application& Get() { return *s_Instance; }
		inline Windows& GetWindow() { return *m_Window; }
	protected:
		std::unique_ptr<Engine::Windows>m_Window;
		bool m_Running = false;
		static Application* s_Instance;
		LayerStack m_LayerStack;
		ImguiLayer* m_ImGuiLayer = nullptr;
		
		// 帧计时相关变量
		float m_DeltaTime = 0.0f;
		float m_LastFrameTime = 0.0f;
		
		// 鼠标控制相关变量
		bool m_FirstMouse = true;
	};
	
	// 由客户端实现
	Application* createApplication();

}

