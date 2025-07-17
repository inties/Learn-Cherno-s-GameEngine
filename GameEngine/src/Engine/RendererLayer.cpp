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
		
		// 设置立方体
		SetupCube();
		
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

	void RendererLayer::SetupCube()
	{
		// 简化的立方体顶点数据 - 只需要8个顶点（每个角一个）
		// 每个顶点包含：位置(x,y,z) + 颜色(r,g,b,a)
		float cubeVertices[] = {
			// 位置                颜色
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // 0: 左下后 - 红色
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  // 1: 右下后 - 绿色
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // 2: 右上后 - 蓝色
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  // 3: 左上后 - 黄色
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  // 4: 左下前 - 紫色
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  // 5: 右下前 - 青色
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,  // 6: 右上前 - 白色
			-0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f   // 7: 左上前 - 灰色
		};

		// 立方体索引数据 - 36个索引组成12个三角形（6个面）
		unsigned int cubeIndices[] = {
			// 后面 (z = -0.5)
			0, 1, 2,  2, 3, 0,
			// 前面 (z = 0.5)
			4, 5, 6,  6, 7, 4,
			// 左面 (x = -0.5)
			0, 4, 7,  7, 3, 0,
			// 右面 (x = 0.5)
			1, 5, 6,  6, 2, 1,
			// 下面 (y = -0.5)
			0, 1, 5,  5, 4, 0,
			// 上面 (y = 0.5)
			3, 2, 6,  6, 7, 3
		};
		
		// 创建顶点缓冲区
		m_VertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		m_VertexBuffer->SetLayout(layout);

		// 创建顶点数组
		m_VertexArray = VertexArray::Create();
		m_VertexArray->SetVertexBuffer(m_VertexBuffer);

		// 创建索引缓冲区
		m_IndexBuffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		// 创建着色器
		m_Shader = Shader::CreateFromFiles("VertexPosColor", GetShaderPath("vertex_shader.glsl"), GetShaderPath("fragment_color.glsl"));
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

		// 创建模型变换矩阵
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, m_Settings.position);
		modelMatrix = glm::rotate(modelMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, m_Settings.scale);

		
		// 从摄像机获取变换矩阵
		m_ViewMatrix=Application::Get().mainCam->GetViewMatrix();
		// 计算最终的 MVP 矩阵
		glm::mat4 viewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;
		glm::mat4 mvpMatrix = viewProjMatrix * modelMatrix;

		// 设置着色器uniform
		m_Shader->Bind();
		m_Shader->SetMat4("u_Transform", mvpMatrix);

		// 提交渲染
		Renderer::Submit(m_Shader, m_VertexArray, modelMatrix);

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
			ImGui::ColorEdit3("Cube Color", &m_Settings.cubeColor.x);
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
			
			// 统计信息
			ImGui::Text("Statistics:");
			ImGui::Text("FPS: %.1f", m_FPS);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / m_FPS);
			ImGui::Text("Current Rotation: %.2f", m_Rotation);
			ImGui::Text("Runtime: %.2f seconds", m_Time);
			
			// 重置按钮
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
		// 设置一个简单的视图矩阵，相机在 (0, 0, 3) 位置，看向原点
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		
		m_ViewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
	}
} 