#include "pch.h"
#include "RendererLayer.h"
#include "Renderer/RenderCommand.h"
#include "platform/OpenGL/OpenGLShader.h"
#include "Renderer/Texture.h"
#include "Model/Model.h"
#include "Resources/ShaderLibrary.h"
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
		
		// ����Ĭ����ɫ��
		LoadDefaultShaders();
		
		// ����������
		SetupCube();
		
		// ����ģ��
		SetupModel();
		
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

	void RendererLayer::SetupModel()
	{
		// ���ر���ģ��
		std::string modelPath = "E:/myGitRepos/myLearnOpengl/models/backpack/backpack.obj";
		ENGINE_CORE_INFO("Loading model from: {}", modelPath);
		
		m_Model = Model::Create(modelPath);
		if (!m_Model) {
			ENGINE_CORE_ERROR("Failed to load model: {}", modelPath);
			return;
		}
		
		ENGINE_CORE_INFO("Successfully loaded model");
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

		// ��Ⱦ������
		if (m_CubeShader && m_CubeVAO) {
			// ����������任����
			glm::mat4 cubeMatrix = glm::mat4(1.0f);
			cubeMatrix = glm::translate(cubeMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
			cubeMatrix = glm::rotate(cubeMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
			cubeMatrix = glm::scale(cubeMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

			// ����ؼ�������Ϣ
			static int debugCount = 0;
			if (debugCount++ < 60) { // ֻ���ǰ60֡����ˢ��
				ENGINE_CORE_INFO("=== Frame {} Debug Info ===", debugCount);
				ENGINE_CORE_INFO("Cube Position: ({:.2f}, {:.2f}, {:.2f})", 
					m_Settings.position.x, m_Settings.position.y, m_Settings.position.z);
				ENGINE_CORE_INFO("Cube Scale: ({:.2f}, {:.2f}, {:.2f})", 
					m_Settings.scale.x, m_Settings.scale.y, m_Settings.scale.z);
				ENGINE_CORE_INFO("FOV: {:.1f}, Near: {:.3f}, Far: {:.1f}", m_FOV, m_NearPlane, m_FarPlane);
			}

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
			
		} else {
			ENGINE_CORE_ERROR("Cube shader or VAO not available");
		}

		// ��ѡ��ͬʱ��Ⱦģ�ͣ�ע�͵���ֻ��ʾ�����壩
		
		if (m_Model) {
			// ����ģ�ͱ任����
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f)); // ƫ��ģ��λ��
			modelMatrix = glm::rotate(modelMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // ��Сģ��

			if (m_UseDebugShader && m_DebugShader) {
				RenderModelWithDebugShader(modelMatrix);
			} else {
				m_Model->SetGlobalTransform(modelMatrix);
				m_Model->Draw();
			}
		}
		

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
			
			// ͳ����Ϣ
			ImGui::Text("Statistics:");
			ImGui::Text("FPS: %.1f", m_FPS);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / m_FPS);
			ImGui::Text("Current Rotation: %.2f", m_Rotation);
			ImGui::Text("Runtime: %.2f seconds", m_Time);
			
			if (m_CubeVAO && m_CubeShader) {
				ImGui::Text("Cube: Ready");
				ImGui::Text("Cube Indices: %d", m_CubeIBO ? m_CubeIBO->GetCount() : 0);
			} else {
				ImGui::Text("Cube: NOT Ready");
			}
			
			if (m_Model) {
				ImGui::Text("Model loaded: YES");
			} else {
				ImGui::Text("Model loaded: NO");
			}
			
			ImGui::Separator();
			
			// ���ð�ť
			if (ImGui::Button("Reset Cube Transform"))
			{
				m_Settings.position = glm::vec3(0.0f);
				m_Settings.scale = glm::vec3(1.0f);
				m_Settings.rotationSpeed = 1.0f;
				m_Rotation = 0.0f;
			}
			
			// ���ٵ��԰�ť
			if (ImGui::Button("Reset to Default View"))
			{
				m_Settings.position = glm::vec3(0.0f, 0.0f, -2.0f);
				m_Settings.scale = glm::vec3(1.0f); // �����屣��ԭ��С
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
		//if (!m_DebugShader || !m_Model) return;

		//auto sharedVAO = m_Model->GetSharedVAO();
		//if (!sharedVAO) {
		//	ENGINE_CORE_ERROR("Model has no shared VAO");
		//	return;
		//}

		//// �󶨵�����ɫ��
		//m_DebugShader->Bind();
		//m_DebugShader->SetMat4("u_Model", modelMatrix);
		//m_DebugShader->SetMat4("u_ViewProjection", viewProjMatrix);
		//m_DebugShader->SetInt("u_DebugMode", m_DebugMode);
		//
		//// ����Ĭ�ϲ�����ɫ
		//m_DebugShader->SetFloat4("u_DiffuseColor", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

		//// �󶨹���VAO����Ⱦ����ģ��
		//sharedVAO->Bind();
		//
		//// ��������ʹ�ù���VAO����Ҫ��Ⱦ��������
		//auto indexBuffer = sharedVAO->GetIndexBuffer();
		//if (indexBuffer) {
		//	RenderCommand::DrawIndexed(sharedVAO);
		//	ENGINE_CORE_INFO("Debug rendered model with {} indices", indexBuffer->GetCount());
		//} else {
		//	ENGINE_CORE_ERROR("Shared VAO has no index buffer");
		//}
	}
} 