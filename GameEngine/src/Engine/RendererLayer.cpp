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

	void RendererLayer::OnAttach()
	{
		// 初始化渲染器
		Renderer::Init();
		
		// 加载默认着色器
		LoadDefaultShaders();
		
		// 设置立方体
		SetupCube();
		
		// 设置模型
		SetupModel();
		
		// 初始化相机和投影矩阵
		m_AspectRatio = 1.0f; // 默认比例，会在第一次窗口大小调整时更新
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
		// 加载默认PBR着色器到ShaderLibrary
		std::string shaderPath = GetShaderPath("default.glsl");
		ENGINE_CORE_INFO("Loading default shader from: {}", shaderPath);
		
		auto defaultShader = ShaderLibrary::Get()->Load("DefaultPBR", shaderPath);
		if (!defaultShader) {
			ENGINE_CORE_ERROR("Failed to load DefaultPBR shader");
		} else {
			ENGINE_CORE_INFO("Successfully loaded DefaultPBR shader");
		}
		
		// 也注册为"Default"以防万一
		ShaderLibrary::Get()->Add("Default", defaultShader);
		
		// 加载测试着色器
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
		// 立方体顶点数据：位置(x, y, z) + 颜色(r, g, b, a)
		float cubeVertices[] = {
			// 位置           颜色 (RGBA)
			// 前面 (z = 0.5)
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, // 0 - 红色
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, // 1 - 绿色
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, // 2 - 蓝色
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f, // 3 - 黄色
			// 后面 (z = -0.5)
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f, // 4 - 洋红
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f, // 5 - 青色
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, // 6 - 白色
			-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f  // 7 - 灰色
		};

		unsigned int cubeIndices[] = {
			// 前面
			0, 1, 2,  2, 3, 0,
			// 后面
			4, 5, 6,  6, 7, 4,
			// 左面
			4, 0, 3,  3, 7, 4,
			// 右面
			1, 5, 6,  6, 2, 1,
			// 下面
			4, 5, 1,  1, 0, 4,
			// 上面
			3, 2, 6,  6, 7, 3
		};

		// 创建VAO
		m_CubeVAO = VertexArray::Create();
		
		// 创建VBO
		m_CubeVBO = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		m_CubeVBO->SetLayout(layout);
		m_CubeVAO->SetVertexBuffer(m_CubeVBO);

		// 创建IBO
		m_CubeIBO = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		m_CubeVAO->SetIndexBuffer(m_CubeIBO);

		// 创建立方体着色器
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
		// 加载背包模型
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
		// 更新时间
		m_Time = static_cast<float>(glfwGetTime());
		
		// 更新旋转
		m_Rotation += m_Settings.rotationSpeed * 0.016f; // 假设60FPS
		
		// 设置清屏颜色
		RenderCommand::SetClearColor(glm::vec4(m_Settings.clearColor, 1.0f));
		RenderCommand::Clear();

		// 开始场景
		Renderer::BeginScene();

		// 渲染立方体
		if (m_CubeShader && m_CubeVAO) {
			// 创建立方体变换矩阵
			glm::mat4 cubeMatrix = glm::mat4(1.0f);
			cubeMatrix = glm::translate(cubeMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
			cubeMatrix = glm::rotate(cubeMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
			cubeMatrix = glm::scale(cubeMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

			// 输出关键调试信息
			static int debugCount = 0;
			if (debugCount++ < 60) { // 只输出前60帧避免刷屏
				ENGINE_CORE_INFO("=== Frame {} Debug Info ===", debugCount);
				ENGINE_CORE_INFO("Cube Position: ({:.2f}, {:.2f}, {:.2f})", 
					m_Settings.position.x, m_Settings.position.y, m_Settings.position.z);
				ENGINE_CORE_INFO("Cube Scale: ({:.2f}, {:.2f}, {:.2f})", 
					m_Settings.scale.x, m_Settings.scale.y, m_Settings.scale.z);
				ENGINE_CORE_INFO("FOV: {:.1f}, Near: {:.3f}, Far: {:.1f}", m_FOV, m_NearPlane, m_FarPlane);
			}

			// 获取视图矩阵
			glm::mat4 viewMatrix = m_ViewMatrix;
			Camera* camera = Camera::GetInstance();
			if (camera) {
				viewMatrix = camera->GetViewMatrix();
			}
			glm::mat4 viewProjMatrix = m_ProjectionMatrix * viewMatrix;

			// 绑定立方体着色器并设置uniforms
			m_CubeShader->Bind();
			m_CubeShader->SetMat4("u_Model", cubeMatrix);
			m_CubeShader->SetMat4("u_ViewProjection", viewProjMatrix);

			// 绑定VAO并渲染立方体
			m_CubeVAO->Bind();
			RenderCommand::DrawIndexed(m_CubeVAO);
			
		} else {
			ENGINE_CORE_ERROR("Cube shader or VAO not available");
		}

		// 可选：同时渲染模型（注释掉以只显示立方体）
		
		if (m_Model) {
			// 创建模型变换矩阵
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f)); // 偏移模型位置
			modelMatrix = glm::rotate(modelMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // 缩小模型

			if (m_UseDebugShader && m_DebugShader) {
				RenderModelWithDebugShader(modelMatrix);
			} else {
				m_Model->SetGlobalTransform(modelMatrix);
				m_Model->Draw();
			}
		}
		

		// 结束场景
		Renderer::EndScene();
		
		// 更新FPS统计
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
		// 创建渲染器设置窗口
		if (ImGui::Begin("RenderInfo"))
		{
			ImGui::Text("Renderer Layer Properties");
			ImGui::Separator();
			
			// 基本渲染设置
			ImGui::Text("Basic Settings:");
			ImGui::ColorEdit3("Clear Color", &m_Settings.clearColor.x);
			ImGui::Checkbox("Wireframe", &m_Settings.wireframe);
			
			ImGui::Separator();
			
			// 立方体变换设置
			ImGui::Text("Cube Transform:");
			ImGui::SliderFloat("Rotation Speed", &m_Settings.rotationSpeed, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat3("Position", &m_Settings.position.x, -5.0f, 5.0f, "%.2f");
			ImGui::SliderFloat3("Scale", &m_Settings.scale.x, 0.1f, 3.0f, "%.2f");
			
			ImGui::Separator();
			
			// 相机设置
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
			
			// 调试选项
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
			
			// 相机信息
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
			
			// 统计信息
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
			
			// 重置按钮
			if (ImGui::Button("Reset Cube Transform"))
			{
				m_Settings.position = glm::vec3(0.0f);
				m_Settings.scale = glm::vec3(1.0f);
				m_Settings.rotationSpeed = 1.0f;
				m_Rotation = 0.0f;
			}
			
			// 快速调试按钮
			if (ImGui::Button("Reset to Default View"))
			{
				m_Settings.position = glm::vec3(0.0f, 0.0f, -2.0f);
				m_Settings.scale = glm::vec3(1.0f); // 立方体保持原大小
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
		
		// 更新纵横比
		m_AspectRatio = static_cast<float>(e.GetWindowWidth()) / static_cast<float>(e.GetWindowHeight());
		
		// 重新计算投影矩阵
		UpdateProjectionMatrix();
		
		// 不拦截事件，让其他层也能处理
		return false;
	}

	void RendererLayer::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	void RendererLayer::SetupViewMatrix()
	{
		// 强制设置相机到安全位置
		Camera* camera = Camera::GetInstance();
		if (camera) {
			ENGINE_CORE_INFO("Force setting camera to safe position");
			camera->SetPosition(glm::vec3(0.0f, 0.0f, 50.0f));
		}
		
		// 设置一个简单的视图矩阵，相机在 (0, 0, 50) 位置，看向原点
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

		//// 绑定调试着色器
		//m_DebugShader->Bind();
		//m_DebugShader->SetMat4("u_Model", modelMatrix);
		//m_DebugShader->SetMat4("u_ViewProjection", viewProjMatrix);
		//m_DebugShader->SetInt("u_DebugMode", m_DebugMode);
		//
		//// 设置默认材质颜色
		//m_DebugShader->SetFloat4("u_DiffuseColor", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

		//// 绑定共享VAO并渲染整个模型
		//sharedVAO->Bind();
		//
		//// 由于我们使用共享VAO，需要渲染所有索引
		//auto indexBuffer = sharedVAO->GetIndexBuffer();
		//if (indexBuffer) {
		//	RenderCommand::DrawIndexed(sharedVAO);
		//	ENGINE_CORE_INFO("Debug rendered model with {} indices", indexBuffer->GetCount());
		//} else {
		//	ENGINE_CORE_ERROR("Shared VAO has no index buffer");
		//}
	}
} 