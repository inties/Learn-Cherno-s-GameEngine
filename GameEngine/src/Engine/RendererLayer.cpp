#include "pch.h"
#include "RendererLayer.h"
#include "Renderer/RenderCommand.h"
#include "platform/OpenGL/OpenGLShader.h"
#include "Renderer/Texture.h"
// #include "Model/Model.h"
#include "Resources/ShaderLibrary.h"
#include "Resources/ModelManager.h"
#include "Scene/Scene.h"
#include <imgui.h>
#include <filesystem>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Scene/Component.h"
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
		
		// 移除硬编码模型加载（改由 Scene + ModelManager 驱动）
		// SetupModel();
		const std::string s= "backpack.obj";
		m_model = ModelManager::Get()->LoadModel(s);
		if (m_model) {
			std::cout << "加载成功" << std::endl;
		}
		// 初始化相机和投影矩阵
		m_AspectRatio = 1.0f; // 默认比例，会在第一次窗口大小调整时更新
		UpdateProjectionMatrix();
		SetupViewMatrix();

		// 创建离屏渲染目标（使用当前窗口尺寸作为初始尺寸）
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		ENGINE_CORE_INFO("target x,y,{},{}", initW, initH);
		//CreateRenderTarget(initW, initH);
		typedef FramebufferTextureFormat format;
		FramebufferSpecification spec = { initW,initH,{format::RGBA8,format::RED_INTEGER,format::DEPTH24STENCIL8},1 };
		FBO = Framebuffer::Create(spec);
		ENGINE_CORE_INFO("RendererLayer attached");
	}

	void RendererLayer::OnDetach()
	{
		ENGINE_CORE_INFO("RendererLayer detached");
		//DestroyRenderTarget();
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
		auto CubeVAO = VertexArray::Create();
		
		// 创建VBO
		auto CubeVBO = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		CubeVBO->SetLayout(layout);
		CubeVAO->SetVertexBuffer(CubeVBO);

		// 创建IBO
		auto CubeIBO = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		CubeVAO->SetIndexBuffer(CubeIBO);
		VAO_Manager.Regist("cube", CubeVAO);
		// 创建立方体着色器
		std::string cubeShaderPath = GetShaderPath("cube.glsl");
		//m_CubeShader = Shader::Create(cubeShaderPath);
		auto CubeShader = Shader::Create(cubeShaderPath);
		auto CubeDefaultMat = Material::Create(CubeShader);
		if (!CubeShader) {
			ENGINE_CORE_ERROR("Failed to load cube shader");
		}
		else {
			ENGINE_CORE_INFO("Successfully loaded cube shader");
		}
		Mat_Manager.Regist("cube", CubeDefaultMat);
		
	}

	void RendererLayer::OnUpdate()
	{
		// 更新时间
		m_Time = static_cast<float>(glfwGetTime());
		
		// 更新旋转
		m_Rotation += m_Settings.rotationSpeed * 0.016f; // 假设60FPS
		
		// 设置线框模式
		if (m_Settings.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		FBO->Bind();
		// 绑定离屏帧缓冲并设置视口
		//glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		RenderCommand::SetViewport(0, 0, GetRenderWidth(), GetRenderHeight());
		//glViewport(0, 0, (GLint)m_RTWidth, (GLint)m_RTHeight);
		 
		// 设置清屏颜色并清屏
		RenderCommand::SetClearColor(glm::vec4(m_Settings.clearColor, 1.0f));
		RenderCommand::Clear();
		FBO->ClearAttachment(1, 0);
		FBO->ClearAttachment(0, 0);


		// 开始场景
		Renderer::BeginScene();
		//m_CubeVAO = VAO_Manager.Get("cube");
		//m_CubeShader = Mat_Manager.Get("cube")->GetShader();
	//	// 渲染立方体
	//if (m_ShowCube && m_CubeShader && m_CubeVAO) {
	//		// 创建立方体变换矩阵
	//		glm::mat4 cubeMatrix = glm::mat4(1.0f);
	//		cubeMatrix = glm::translate(cubeMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
	//		cubeMatrix = glm::rotate(cubeMatrix, m_Rotation, glm::vec3(1.0f, 1.0f, 0.0f));
	//		cubeMatrix = glm::scale(cubeMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

	//		// 获取视图矩阵
	//		glm::mat4 viewMatrix = m_ViewMatrix;
	//		Camera* camera = Camera::GetInstance();
	//		if (camera) {
	//			viewMatrix = camera->GetViewMatrix();
	//		}
	//		glm::mat4 viewProjMatrix = m_ProjectionMatrix * viewMatrix;

	//		// 绑定立方体着色器并设置uniforms
	//		m_CubeShader->Bind();
	//		m_CubeShader->SetMat4("u_Model", cubeMatrix);
	//		m_CubeShader->SetMat4("u_ViewProjection", viewProjMatrix);

	//		// 绑定VAO并渲染立方体
	//		m_CubeVAO->Bind();
	//		RenderCommand::DrawIndexed(m_CubeVAO);
	//	}

		DrawRenderItems();
		// 渲染场景中的模型实例（保留原有逻辑）
		const auto& objects = m_Scene->GetGameObjects();
		for (size_t i = 0; i < objects.size(); ++i) {
			const auto& obj = objects[i];
			if (auto shared_model = obj.model.lock()) {
				// 为每个mesh设置物体ID
				shared_model->SetObjectID(i);
				// 使用场景对象的变换渲染
				shared_model->SetGlobalTransform(obj.transform);
				shared_model->Draw();
			}
			else {
				ENGINE_CORE_WARN("Failed to load model for scene object: {}", obj.modelPath);
				continue;
			}
		}
		

		// 结束场景
		Renderer::EndScene();

		// 解绑帧缓冲，恢复到默认帧缓冲，并将视口还原为窗口大小，供 ImGui 使用
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		FBO->Unbind();
		unsigned int winW = Application::Get().GetWindow().GetWidth();
		unsigned int winH = Application::Get().GetWindow().GetHeight();
		//glViewport(0, 0, (GLint)winW, (GLint)winH);
		RenderCommand::SetViewport(0, 0, (GLint)winW, (GLint)winH);
		
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
			ImGui::Checkbox("Show Cube", &m_ShowCube);
			
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
			
			// 场景信息
			if (m_Scene) {
				ImGui::Text("Scene Objects: %d", (int)m_Scene->GetGameObjects().size());
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
		
		// 更新纵横比（窗口尺寸变化时，仅更新投影矩阵；渲染目标由 Editor 控制）
		m_AspectRatio = static_cast<float>(e.GetWindowWidth()) / static_cast<float>(e.GetWindowHeight());
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
		// (kept commented out debug rendering)
	}

	void RendererLayer::DrawRenderItems()
	{
		// 使用ECS系统渲染有RenderComponent的实体
		if (!m_Scene) return;
		// 获取场景的registry
		auto& registry = m_Scene->GetRegistry();

		// 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
		auto view = registry.view<RenderComponent, TransformComponent>();

		for (auto& [entity, renderComp, TransComp] : view.each()) {

			// 绑定资源（如果还未绑定）
			if (!renderComp.IsValid()) {
				renderComp.BindResources(VAO_Manager, Mat_Manager);
			}

			// 如果资源绑定成功，进行渲染
			if (renderComp.IsValid()) {
				// 创建变换矩阵
				glm::mat4 modelMatrix = TransComp.GetTransform();

				// 获取视图矩阵
				Camera* camera = Camera::GetInstance();
				ENGINE_CORE_INFO("{}Zoom", camera->Zoom);
				glm::mat4 viewMatrix = camera->GetViewMatrix();
				glm::mat4 projMatrix = camera->GetProjectionMatrix();
				glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

				// 绑定着色器并设置uniforms
				auto shader = renderComp.Mat->GetShader();
				shader->Bind();
				shader->SetMat4("u_Model", modelMatrix);
				shader->SetMat4("u_ViewProjection", viewProjMatrix);

				// 绑定VAO并渲染
				renderComp.VAO->Bind();
				RenderCommand::DrawIndexed(renderComp.VAO);
			}
		}
		

	}

	//void RendererLayer::CreateRenderTarget(unsigned int width, unsigned int height)
	//{
	//	DestroyRenderTarget();
	//	m_RTWidth = width; m_RTHeight = height;

	//	glGenFramebuffers(1, &m_Framebuffer);
	//	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	//	// Color attachment texture
	//	glGenTextures(1, &m_ColorAttachment);
	//	glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

	//	// Depth-stencil renderbuffer
	//	glGenRenderbuffers(1, &m_DepthStencilRBO);
	//	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRBO);
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height);
	//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRBO);

	//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	//		ENGINE_CORE_ERROR("Framebuffer is not complete!");
	//	}

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

	//void RendererLayer::DestroyRenderTarget()
	//{
	//	if (m_DepthStencilRBO) { glDeleteRenderbuffers(1, &m_DepthStencilRBO); m_DepthStencilRBO = 0; }
	//	if (m_ColorAttachment) { glDeleteTextures(1, &m_ColorAttachment); m_ColorAttachment = 0; }
	//	if (m_Framebuffer) { glDeleteFramebuffers(1, &m_Framebuffer); m_Framebuffer = 0; }
	//}

	void RendererLayer::ResizeRenderTarget(unsigned int width, unsigned int height)
	{
		/*if (width == 0 || height == 0) return;
		if (width == m_RTWidth && height == m_RTHeight) return;
		CreateRenderTarget(width, height);
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		UpdateProjectionMatrix();*/
		FBO->Resize(width, height);
	}

	void RendererLayer::SetObjectIDForModel(const Ref<Model>& model, int objectID)
	{
		if (model) {
			model->SetObjectID(objectID);
		}
	}
}