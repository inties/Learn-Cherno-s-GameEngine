#pragma once
#include"pch.h"
#include"Engine/Layer.h"
#include"Engine/Event.h"
namespace Engine
{
	class ENGINE_API ImguiLayer :public Layer {
	public:
		ImguiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		//virtual void OnImGuiRender()override;
		virtual void OnEvent(Event& event)override;
	public:
		float m_time = 0.0f;

	private:
		// 鼠标事件处理
		bool OnMouseMove(MouseMoveEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		
		// 键盘事件处理
		bool OnKeyPressed(KeyPressEvent& e);
		bool OnKeyReleased(KeyReleaseEvent& e);
		
		// 窗口事件处理
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		
		// ImGui 状态管理
		bool m_BlockEvents = true;  // 是否阻止事件继续传播
		bool m_ShowDemoWindow = false;  // 是否显示演示窗口
		


};

}