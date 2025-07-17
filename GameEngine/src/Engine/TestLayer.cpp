#include "pch.h"
#include "TestLayer.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

TestLayer::TestLayer()
	: Layer("TestLayer")
{
}

void TestLayer::OnAttach()
{
	ENGINE_CORE_INFO("TestLayer attached");
}

void TestLayer::OnDetach()
{
	ENGINE_CORE_INFO("TestLayer detached");
}

void TestLayer::OnUpdate()
{
	// ����ʱ��
	m_Time = static_cast<float>(glfwGetTime());
	
	// TestLayer ���ڲ�������Ⱦ��ֻ������Ϸ�߼�
	// ��Ⱦ�� RendererLayer ����
}

void TestLayer::OnImGuiRender()
{
	// ��ʾһ���򵥵Ĳ��Դ���
	if (ImGui::Begin("Test Layer"))
	{
		ImGui::Text("This is the Test Layer");
		ImGui::Text("Runtime: %.2f seconds", m_Time);
		
		if (ImGui::Button("Click me!"))
		{
			m_Counter++;
			ENGINE_CORE_INFO("Button clicked {} times", m_Counter);
		}
		
		ImGui::Text("Button clicked %d times", m_Counter);
		
		ImGui::Separator();
		ImGui::Text("Note: Rendering is now handled by RendererLayer");
	}
	ImGui::End();
}

void TestLayer::OnEvent(Engine::Event& e)
{
	// ����һЩ�����¼�
	ENGINE_CORE_TRACE("TestLayer received event: {}", e.GetName());
} 