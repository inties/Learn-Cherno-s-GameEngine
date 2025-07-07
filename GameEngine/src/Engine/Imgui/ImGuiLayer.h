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
		// ����¼�����
		bool OnMouseMove(MouseMoveEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		
		// �����¼�����
		bool OnKeyPressed(KeyPressEvent& e);
		bool OnKeyReleased(KeyReleaseEvent& e);
		
		// �����¼�����
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		
		// ImGui ״̬����
		bool m_BlockEvents = true;  // �Ƿ���ֹ�¼���������
		bool m_ShowDemoWindow = false;  // �Ƿ���ʾ��ʾ����
		


};

}