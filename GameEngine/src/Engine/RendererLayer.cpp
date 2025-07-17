#include "pch.h"
#include "RendererLayer.h"
#include "Renderer/RenderCommand.h"
#include "platform/OpenGL/OpenGLShader.h"
#include <imgui.h>
#include <filesystem>
#include <GLFW/glfw3.h>

namespace Engine
{
	RendererLayer::RendererLayer()
		: Layer("RendererLayer")
	{
	}

	std::string RendererLayer::GetShaderPath(const std::string& filename)
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

	void RendererLayer::OnAttach()
	{
		// ��ʼ����Ⱦ��
		Renderer::Init();
		
		// ����������
		SetupCube();
		
		// ��ʼ�������ͶӰ����
		m_AspectRatio = 1.0f; // Ĭ�ϱ��������ڵ�һ�δ��ڴ�С����ʱ����
		UpdateProjectionMatrix();
		SetupViewMatrix();
		
		ENGINE_CORE_INFO("RendererLayer attached");
	}

	void RendererLayer::OnDetach()
	{
		ENGINE_CORE_INFO("RendererLayer detached");
	}

	void RendererLayer::SetupCube()
	{
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
		m_VertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		m_VertexBuffer->SetLayout(layout);

		// ������������
		m_VertexArray = VertexArray::Create();
		m_VertexArray->SetVertexBuffer(m_VertexBuffer);

		// ��������������
		m_IndexBuffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		// ������ɫ��
		m_Shader = Shader::CreateFromFiles("VertexPosColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_color.glsl"));
	}

	void RendererLayer::OnUpdate()
	{
		// ����ʱ��
		m_Time = static_cast<float>(glfwGetTime());
		
		// ������ת
		m_Rotation += m_Settings.rotationSpeed * 0.016f; // ����60FPS
		
		// ����������ɫ
		RenderCommand::SetClearColor(glm::vec4(m_Settings.clearColor, 1.0f));
		RenderCommand::Clear();

		// ��ʼ����
		Renderer::BeginScene();

		// ����ģ�ͱ任����
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, m_Settings.position);
		modelMatrix = glm::rotate(modelMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, m_Settings.scale);

		
		// ���������ȡ�任����
		m_ViewMatrix=Application::Get().mainCam->GetViewMatrix();
		// �������յ� MVP ����
		glm::mat4 viewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;
		glm::mat4 mvpMatrix = viewProjMatrix * modelMatrix;

		// ������ɫ��uniform
		m_Shader->Bind();
		m_Shader->SetMat4("u_Transform", mvpMatrix);

		// �ύ��Ⱦ
		Renderer::Submit(m_Shader, m_VertexArray, modelMatrix);

		// ��������
		Renderer::EndScene();
		
		// ����FPSͳ��
		m_FrameCount++;
		if (m_Time - m_LastFPSUpdate >= 1.0f)
		{
			m_FPS = m_FrameCount / (m_Time - m_LastFPSUpdate);
			m_FrameCount = 0;
			m_LastFPSUpdate = m_Time;
		}
	}

	void RendererLayer::OnImGuiRender()
	{
		// ������Ⱦ�����ô���
		if (ImGui::Begin("RenderInfo"))
		{
			ImGui::Text("Renderer Layer Properties");
			ImGui::Separator();
			
			// ������Ⱦ����
			ImGui::Text("Basic Settings:");
			ImGui::ColorEdit3("Clear Color", &m_Settings.clearColor.x);
			ImGui::ColorEdit3("Cube Color", &m_Settings.cubeColor.x);
			ImGui::Checkbox("Wireframe", &m_Settings.wireframe);
			
			ImGui::Separator();
			
			// ������任����
			ImGui::Text("Cube Transform:");
			ImGui::SliderFloat("Rotation Speed", &m_Settings.rotationSpeed, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat3("Position", &m_Settings.position.x, -5.0f, 5.0f, "%.2f");
			ImGui::SliderFloat3("Scale", &m_Settings.scale.x, 0.1f, 3.0f, "%.2f");
			
			ImGui::Separator();
			
			// �������
			ImGui::Text("Camera Settings:");
			bool projectionChanged = false;
			projectionChanged |= ImGui::SliderFloat("FOV", &m_FOV, 10.0f, 120.0f, "%.1f");
			projectionChanged |= ImGui::SliderFloat("Near Plane", &m_NearPlane, 0.01f, 1.0f, "%.3f");
			projectionChanged |= ImGui::SliderFloat("Far Plane", &m_FarPlane, 10.0f, 1000.0f, "%.1f");
			ImGui::Text("Aspect Ratio: %.3f", m_AspectRatio);
			
			if (projectionChanged) {
				UpdateProjectionMatrix();
			}
			
			ImGui::Separator();
			
			// ͳ����Ϣ
			ImGui::Text("Statistics:");
			ImGui::Text("FPS: %.1f", m_FPS);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / m_FPS);
			ImGui::Text("Current Rotation: %.2f", m_Rotation);
			ImGui::Text("Runtime: %.2f seconds", m_Time);
			
			// ���ð�ť
			if (ImGui::Button("Reset Transform"))
			{
				m_Settings.position = glm::vec3(0.0f);
				m_Settings.scale = glm::vec3(1.0f);
				m_Settings.rotationSpeed = 1.0f;
				m_Rotation = 0.0f;
			}
		}
		ImGui::End();
	}

	void RendererLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) -> bool {
			return this->OnWindowResize(e);
		});
	}

	bool RendererLayer::OnWindowResize(WindowResizeEvent& e)
	{
		ENGINE_CORE_INFO("RendererLayer: Updating projection matrix for {}x{}", e.GetWindowWidth(), e.GetWindowHeight());
		
		// �����ݺ��
		m_AspectRatio = static_cast<float>(e.GetWindowWidth()) / static_cast<float>(e.GetWindowHeight());
		
		// ���¼���ͶӰ����
		UpdateProjectionMatrix();
		
		// �������¼�����������Ҳ�ܴ���
		return false;
	}

	void RendererLayer::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	void RendererLayer::SetupViewMatrix()
	{
		// ����һ���򵥵���ͼ��������� (0, 0, 3) λ�ã�����ԭ��
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		
		m_ViewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
	}
} 