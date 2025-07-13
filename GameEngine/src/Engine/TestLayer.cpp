#include "pch.h"
#include "TestLayer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "platform/OpenGL/OpenGLShader.h"
#include <imgui.h>
#include <filesystem>

TestLayer::TestLayer()
	: Layer("TestLayer")
{
}

std::string TestLayer::GetShaderPath(const std::string& filename)
{
	// 尝试多个可能的路径
	std::vector<std::string> possiblePaths = {
		"GameEngine/Shader/" + filename,
		"../GameEngine/Shader/" + filename,
		"../../GameEngine/Shader/" + filename,
		"../../../GameEngine/Shader/" + filename,
		"../../../../GameEngine/Shader/" + filename
	};
	
	for (const auto& path : possiblePaths) {
		if (std::filesystem::exists(path)) {
			ENGINE_CORE_INFO("Found shader file: {}", path);
			return path;
		}
	}
	
	ENGINE_CORE_ERROR("Could not find shader file: {}", filename);
	return "GameEngine/Shader/" + filename; // 回退到默认路径
}

void TestLayer::OnAttach()
{
	// 创建顶点数据 - 一个彩色三角形
	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,  // 左下角 - 紫色
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,  // 右下角 - 蓝色
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f   // 顶部 - 黄色
	};

	// 创建顶点缓冲区
	m_VertexBuffer = Engine::VertexBuffer::Create(vertices, sizeof(vertices));
	Engine::BufferLayout layout = {
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float4, "a_Color" }
	};
	m_VertexBuffer->SetLayout(layout);

	// 创建顶点数组
	m_VertexArray = Engine::VertexArray::Create();
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	// 创建索引数据
	uint32_t indices[3] = { 0, 1, 2 };
	m_IndexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);

	// 创建位置颜色着色器 - 从文件路径创建
	m_Shader = Engine::Shader::CreateFromFiles("VertexPosColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_position.glsl"));

	// 创建顶点颜色着色器 - 从文件路径创建
	m_BlueShader = Engine::Shader::CreateFromFiles("VertexColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_color.glsl"));
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate()
{
	// 设置清屏颜色
	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Engine::RenderCommand::Clear();

	// 开始场景渲染
	Engine::Renderer::BeginScene();
	
	// 创建绕z轴旋转的变换矩阵
	float time = (float)glfwGetTime(); // 获取当前时间
	float rotationSpeed = 50.0f; // 每秒旋转50度
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 
		glm::radians(time * rotationSpeed), 
		glm::vec3(0.0f, 0.0f, 1.0f)); // 绕z轴旋转
	
	m_Shader->SetMat4("u_Transform", rotation);
	// 提交渲染对象
	Engine::Renderer::Submit(m_Shader, m_VertexArray);

	// 结束场景渲染
	Engine::Renderer::EndScene();
}

void TestLayer::OnImGuiRender()
{
	//ImGui::Begin("Renderer Test");
	//ImGui::Text("GameEngine Renderer Demo");
	//ImGui::Separator();
	//
	//ImGui::Text("Shader Selection:");
	//
	//if (ImGui::Button("Position Color Shader"))
	//{
	//	// 切换到位置颜色着色器（基于顶点位置的颜色）
	//	m_Shader = Engine::Shader::CreateFromFiles("PositionColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_position.glsl"));
	//}
	//
	//if (ImGui::Button("Vertex Color Shader"))
	//{
	//	// 切换到顶点颜色着色器（使用顶点数据中的颜色）
	//	m_Shader = m_BlueShader;
	//}
	//
	//ImGui::Separator();
	//ImGui::Text("Triangle vertices: 3");
	//ImGui::Text("Indices: 3");
	//ImGui::Text("Render API: %s", Engine::Renderer::GetAPI() == Engine::RendererAPI::API::OpenGL ? "OpenGL" : "Unknown");
	//ImGui::Text("Shader files loaded from: GameEngine/Shader/");
	//
	//ImGui::End();
}

void TestLayer::OnEvent(Engine::Event& e)
{
} 