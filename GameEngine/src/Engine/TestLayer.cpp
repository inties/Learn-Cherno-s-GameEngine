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
	// ���Զ�����ܵ�·��
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
	return "GameEngine/Shader/" + filename; // ���˵�Ĭ��·��
}

void TestLayer::OnAttach()
{
	// ������������ - һ����ɫ������
	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,  // ���½� - ��ɫ
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,  // ���½� - ��ɫ
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f   // ���� - ��ɫ
	};
	
	// �򻯵������嶥������ - ֻ��Ҫ8�����㣨ÿ����һ����
	// ÿ�����������λ��(x,y,z) + ��ɫ(r,g,b,a)
	float cubeVertices[] = {
		// λ��                ��ɫ
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // 0: ���º� - ��ɫ
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  // 1: ���º� - ��ɫ
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // 2: ���Ϻ� - ��ɫ
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  // 3: ���Ϻ� - ��ɫ
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  // 4: ����ǰ - ��ɫ
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  // 5: ����ǰ - ��ɫ
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,  // 6: ����ǰ - ��ɫ
		-0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f   // 7: ����ǰ - ��ɫ
	};

	// �������������� - 36���������12�������Σ�6���棩
	unsigned int cubeIndices[] = {
		// ���� (z = -0.5)
		0, 1, 2,  2, 3, 0,
		// ǰ�� (z = 0.5)
		4, 5, 6,  6, 7, 4,
		// ���� (x = -0.5)
		0, 4, 7,  7, 3, 0,
		// ���� (x = 0.5)
		1, 5, 6,  6, 2, 1,
		// ���� (y = -0.5)
		0, 1, 5,  5, 4, 0,
		// ���� (y = 0.5)
		3, 2, 6,  6, 7, 3
	};
	
	// �������㻺����
	m_VertexBuffer = Engine::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
	Engine::BufferLayout layout = {
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float4, "a_Color" }
	};
	m_VertexBuffer->SetLayout(layout);

	// ������������
	m_VertexArray = Engine::VertexArray::Create();
	m_VertexArray->SetVertexBuffer(m_VertexBuffer);

	// ��������������
	m_IndexBuffer = Engine::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);

	// ����λ����ɫ��ɫ�� - ���ļ�·������
	//m_Shader = Engine::Shader::CreateFromFiles("VertexPosColor", "Shader/vertex_shader.glsl", "Shader/fragment_position.glsl");
	m_Shader = Engine::Shader::CreateFromFiles("VertexPosColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_color.glsl"));

	// ����������ɫ��ɫ�� - ���ļ�·������
	m_BlueShader = Engine::Shader::CreateFromFiles("VertexColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_color.glsl"));
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate()
{
	// ����������ɫ
	Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Engine::RenderCommand::Clear();

	// ��ʼ������Ⱦ
	Engine::Renderer::BeginScene();
	
	// ������z����ת�ı任����
	float time = (float)glfwGetTime(); // ��ȡ��ǰʱ��
	float rotationSpeed = 50.0f; // ÿ����ת50��
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 
		glm::radians(time * rotationSpeed), 
		glm::vec3(0.0f, 0.0f, 1.0f)); // ��z����ת
	
	m_Shader->SetMat4("u_Transform", rotation);
	// �ύ��Ⱦ����
	Engine::Renderer::Submit(m_Shader, m_VertexArray);

	// ����������Ⱦ
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
	//	// �л���λ����ɫ��ɫ�������ڶ���λ�õ���ɫ��
	//	m_Shader = Engine::Shader::CreateFromFiles("PositionColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_position.glsl"));
	//}
	//
	//if (ImGui::Button("Vertex Color Shader"))
	//{
	//	// �л���������ɫ��ɫ����ʹ�ö��������е���ɫ��
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