#include "pch.h"
#include "RendererLayer.h"
#include "Renderer/RenderCommand.h"
#include "platform/OpenGL/OpenGLShader.h"
#include "Renderer/Texture.h"
// #include "Model/Model.h"
#include "Resources/ShaderLibrary.h"
#include "Resources/ResourceManager.h"
#include "Scene/Scene.h"
#include <imgui.h>
#include <filesystem>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
		
		// ����Ĭ����ɫ��
		LoadDefaultShaders();
		
		// ����������
		SetupCube();
		
		// �Ƴ�Ӳ����ģ�ͼ��أ����� Scene + ResourceManager ������
		// SetupModel();
		
		// ��ʼ�������ͶӰ����
		m_AspectRatio = 1.0f; // Ĭ�ϱ��������ڵ�һ�δ��ڴ�С����ʱ����
		UpdateProjectionMatrix();
		SetupViewMatrix();

		// ����������ȾĿ�꣨ʹ�õ�ǰ���ڳߴ���Ϊ��ʼ�ߴ磩
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		CreateRenderTarget(initW, initH);
		
		ENGINE_CORE_INFO("RendererLayer attached");
	}

	void RendererLayer::OnDetach()
	{
		ENGINE_CORE_INFO("RendererLayer detached");
		DestroyRenderTarget();
	}

	void RendererLayer::LoadDefaultShaders()
	{
		// ����Ĭ��PBR��ɫ����ShaderLibrary
		std::string shaderPath = GetShaderPath("default.glsl");
		ENGINE_CORE_INFO("Loading default shader from: {}", shaderPath);
		
		auto defaultShader = ShaderLibrary::Get()->Load("DefaultPBR", shaderPath);
		if (!defaultShader) {
			ENGINE_CORE_ERROR("Failed to load DefaultPBR shader");
		} else {
			ENGINE_CORE_INFO("Successfully loaded DefaultPBR shader");
		}
		
		// Ҳע��Ϊ"Default"�Է���һ
		ShaderLibrary::Get()->Add("Default", defaultShader);
		
		// ���ز�����ɫ��
		std::string testShaderPath = GetShaderPath("test_simple.glsl");
		ENGINE_CORE_INFO("Loading test shader from: {}", testShaderPath);
		
		m_DebugShader = Shader::Create(testShaderPath);
		if (!m_DebugShader) {
			ENGINE_CORE_ERROR("Failed to load test shader");
		} else {
			ENGINE_CORE_INFO("Successfully loaded test shader");
		}
	}

	void RendererLayer::SetupCube()
	{
		// �����嶥�����ݣ�λ��(x, y, z) + ��ɫ(r, g, b, a)
		float cubeVertices[] = {
			// λ��           ��ɫ (RGBA)
			// ǰ�� (z = 0.5)
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, // 0 - ��ɫ
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, // 1 - ��ɫ
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, // 2 - ��ɫ
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f, // 3 - ��ɫ
			// ���� (z = -0.5)
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f, // 4 - ���
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f, // 5 - ��ɫ
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, // 6 - ��ɫ
			-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f  // 7 - ��ɫ
		};

		unsigned int cubeIndices[] = {
			// ǰ��
			0, 1, 2,  2, 3, 0,
			// ����
			4, 5, 6,  6, 7, 4,
			// ����
			4, 0, 3,  3, 7, 4,
			// ����
			1, 5, 6,  6, 2, 1,
			// ����
			4, 5, 1,  1, 0, 4,
			// ����
			3, 2, 6,  6, 7, 3
		};

		// ����VAO
		m_CubeVAO = VertexArray::Create();
		
		// ����VBO
		m_CubeVBO = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		m_CubeVBO->SetLayout(layout);
		m_CubeVAO->SetVertexBuffer(m_CubeVBO);

		// ����IBO
		m_CubeIBO = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		m_CubeVAO->SetIndexBuffer(m_CubeIBO);

		// ������������ɫ��
		std::string cubeShaderPath = GetShaderPath("cube.glsl");
		m_CubeShader = Shader::Create(cubeShaderPath);
		if (!m_CubeShader) {
			ENGINE_CORE_ERROR("Failed to load cube shader");
		} else {
			ENGINE_CORE_INFO("Successfully loaded cube shader");
		}
	}

	void RendererLayer::OnUpdate()
	{
		// ����ʱ��
		m_Time = static_cast<float>(glfwGetTime());
		
		// ������ת
		m_Rotation += m_Settings.rotationSpeed * 0.016f; // ����60FPS
		
		// �����߿�ģʽ
		if (m_Settings.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		// ������֡���岢�����ӿ�
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		glViewport(0, 0, (GLint)m_RTWidth, (GLint)m_RTHeight);

		// ����������ɫ������
		RenderCommand::SetClearColor(glm::vec4(m_Settings.clearColor, 1.0f));
		RenderCommand::Clear();

		// ��ʼ����
		Renderer::BeginScene();

		// ��Ⱦ������
	if (m_ShowCube && m_CubeShader && m_CubeVAO) {
			// ����������任����
			glm::mat4 cubeMatrix = glm::mat4(1.0f);
			cubeMatrix = glm::translate(cubeMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
			cubeMatrix = glm::rotate(cubeMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
			cubeMatrix = glm::scale(cubeMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

			// ��ȡ��ͼ����
			glm::mat4 viewMatrix = m_ViewMatrix;
			Camera* camera = Camera::GetInstance();
			if (camera) {
				viewMatrix = camera->GetViewMatrix();
			}
			glm::mat4 viewProjMatrix = m_ProjectionMatrix * viewMatrix;

			// ����������ɫ��������uniforms
			m_CubeShader->Bind();
			m_CubeShader->SetMat4("u_Model", cubeMatrix);
			m_CubeShader->SetMat4("u_ViewProjection", viewProjMatrix);

			// ��VAO����Ⱦ������
			m_CubeVAO->Bind();
			RenderCommand::DrawIndexed(m_CubeVAO);
		}

		// ��Ⱦ�����е�ģ��ʵ��
		if (m_Scene) {
			const auto& objects = m_Scene->GetModelObjects();
			for (const auto& obj : objects) {
				auto model = ResourceManager::Get()->LoadModel(obj.modelPath);
				if (!model) {
					ENGINE_CORE_WARN("Failed to load model for scene object: {}", obj.modelPath);
					continue;
				}
				// ʹ�ó�������ı任��Ⱦ
				model->SetGlobalTransform(obj.transform);
				model->Draw();
			}
		}

		// ��������
		Renderer::EndScene();

		// ���֡���壬�ָ���Ĭ��֡���壬�����ӿڻ�ԭΪ���ڴ�С���� ImGui ʹ��
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		unsigned int winW = Application::Get().GetWindow().GetWidth();
		unsigned int winH = Application::Get().GetWindow().GetHeight();
		glViewport(0, 0, (GLint)winW, (GLint)winH);
		
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
			ImGui::Checkbox("Wireframe", &m_Settings.wireframe);
			ImGui::Checkbox("Show Cube", &m_ShowCube);
			
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
			
			// ����ѡ��
			ImGui::Separator();
			ImGui::Text("Debug Options:");
			ImGui::Checkbox("Use Debug Shader", &m_UseDebugShader);
			
			if (m_UseDebugShader) {
				const char* debugModes[] = {
					"Normal", "Position Colors", "Normal Colors", 
					"Texture Coords", "Solid Red"
				};
				ImGui::Combo("Debug Mode", &m_DebugMode, debugModes, 5);
			}
			
			ImGui::Separator();
			
			// �����Ϣ
			ImGui::Text("Camera Info:");
			Camera* camera = Camera::GetInstance();
			if (camera) {
				ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", 
					camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
				ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", 
					camera->GetFront().x, camera->GetFront().y, camera->GetFront().z);
			} else {
				ImGui::Text("Camera: Not initialized");
			}
			
			ImGui::Separator();
			
			// ������Ϣ
			if (m_Scene) {
				ImGui::Text("Scene Objects: %d", (int)m_Scene->GetModelObjects().size());
			} else {
				ImGui::Text("No Scene set");
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
		
		// �����ݺ�ȣ����ڳߴ�仯ʱ��������ͶӰ������ȾĿ���� Editor ���ƣ�
		m_AspectRatio = static_cast<float>(e.GetWindowWidth()) / static_cast<float>(e.GetWindowHeight());
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
		// ǿ�������������ȫλ��
		Camera* camera = Camera::GetInstance();
		if (camera) {
			ENGINE_CORE_INFO("Force setting camera to safe position");
			camera->SetPosition(glm::vec3(0.0f, 0.0f, 50.0f));
		}
		
		// ����һ���򵥵���ͼ��������� (0, 0, 50) λ�ã�����ԭ��
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		
		m_ViewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
	}

	void RendererLayer::RenderModelWithDebugShader(const glm::mat4& modelMatrix)
	{
		// (kept commented out debug rendering)
	}

	void RendererLayer::CreateRenderTarget(unsigned int width, unsigned int height)
	{
		DestroyRenderTarget();
		m_RTWidth = width; m_RTHeight = height;

		glGenFramebuffers(1, &m_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

		// Color attachment texture
		glGenTextures(1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// Depth-stencil renderbuffer
		glGenRenderbuffers(1, &m_DepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ENGINE_CORE_ERROR("Framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RendererLayer::DestroyRenderTarget()
	{
		if (m_DepthStencilRBO) { glDeleteRenderbuffers(1, &m_DepthStencilRBO); m_DepthStencilRBO = 0; }
		if (m_ColorAttachment) { glDeleteTextures(1, &m_ColorAttachment); m_ColorAttachment = 0; }
		if (m_Framebuffer) { glDeleteFramebuffers(1, &m_Framebuffer); m_Framebuffer = 0; }
	}

	void RendererLayer::ResizeRenderTarget(unsigned int width, unsigned int height)
	{
		if (width == 0 || height == 0) return;
		if (width == m_RTWidth && height == m_RTHeight) return;
		CreateRenderTarget(width, height);
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		UpdateProjectionMatrix();
	}
}