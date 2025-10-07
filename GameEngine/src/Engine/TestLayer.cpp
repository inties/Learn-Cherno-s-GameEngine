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
	// 更新时间
	m_Time = static_cast<float>(glfwGetTime());
	
	// TestLayer 现在不处理渲染，只处理游戏逻辑
	// 渲染由 RendererLayer 负责
}

void TestLayer::OnImGuiRender()
{
	// 显示一个简单的测试窗口
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
	// 处理一些测试事件
	ENGINE_CORE_TRACE("TestLayer received event: {}", e.GetName());
} 