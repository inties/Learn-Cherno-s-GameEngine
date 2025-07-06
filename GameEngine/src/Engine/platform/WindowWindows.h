#pragma once
#include"pch.h"
#include"Engine/Windows.h"
#include"Engine/platform/WindowsInput.h"
namespace Engine {

	class ENGINE_API WindowWindows : public Windows {
	public:
		WindowWindows(WindowsProps& props = WindowsProps());
		virtual ~WindowWindows();
		void Update() override;
		void SetEventCallBack(const EventFuc& callback) override;
		unsigned int GetWidth() override { return m_Props.Width; }
		unsigned int GetHeight() override { return m_Props.Height; }
		GLFWwindow* GetGLWindow() const { return m_Window; }
	private:
		GLFWwindow* m_Window = nullptr; // ´°¿ÚÖ¸Õë
		WindowsInput* m_WindowsInput = nullptr;




	};



}