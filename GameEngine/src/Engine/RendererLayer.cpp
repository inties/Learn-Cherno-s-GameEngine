#include "pch.h"
#include "RendererLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderpass/RenderPipeline.h"
#include "Engine/Renderer/Shader.h"
#include "Resources/ModelManager.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include "Scene/Component.h"
#include "Engine/Utils/Timer.h"
namespace Engine
{
	RendererLayer::RendererLayer()
		: Layer("RendererLayer")
	{
		// 初始化渲染器
		Renderer::Init();

		SetupTexture();
		// 加载默认着色器
		SetUPGeoMetry();
		SetUpShadersMaterials();

		// 设置默认几何体
		if (m_model) {
			std::cout << "加载成功" << std::endl;
		}
		// 初始化相机和投影矩阵
		UpdateProjectionMatrix();
		SetupViewMatrix();

		// 创建离屏渲染目标（使用当前窗口尺寸作为初始尺寸）
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		ENGINE_CORE_INFO("窗口初始 x,y,{},{}", initW, initH);
		//CreateRenderTarget(initW, initH);
		typedef TextureFormat format;
		FramebufferSpecification spec = { initW,initH,{format::RGB16,format::RED_INTEGER,format::DEPTH24STENCIL8},1 };
		FBO = Framebuffer::Create(spec);
		ENGINE_CORE_INFO("RendererLayer attached");
	}

	RendererLayer::~RendererLayer() = default;

	void RendererLayer::Init(Scene* scene)
	{
		SetScene(scene);

	}


	std::string RendererLayer::GetShaderPath(const std::string& filename)
	{
		// 尝试多个可能的路径
		std::vector<std::string> possiblePaths = {
			"Shader/" + filename,
		};
		
		for (const auto& path : possiblePaths) {
			if (std::filesystem::exists(path)) {
				return path;
			}
		}
		
		ENGINE_CORE_ERROR("Could not find shader file: {}", filename);
		return "GameEngine/Shader/" + filename; // 回退到默认路径
	}

	void RendererLayer::OnAttach()
	{
		// 创建渲染管线
		RenderPipeLineSetting renderPipeLineSetting = {&Mat_Manager,&VAO_Manager,&Texture_Manager,&Shader_Manager,FBO.get(),m_Scene};
		m_RenderPipeLine = CreateScope<RenderPipeLine>(renderPipeLineSetting);
		m_RenderPipeLine->Init();

		GameTimer::Init();
	}

	void RendererLayer::OnDetach()
	{
		ENGINE_CORE_INFO("RendererLayer detached");
		//DestroyRenderTarget();
	}

	void RendererLayer::SetUpShadersMaterials()
	{
		// 加载默认PBR着色器到ShaderLibrary
		std::string shaderPath = GetShaderPath("default.glsl");
		auto defaultShader = Shader::Create(shaderPath);

		std::string shaderPath_vs = GetShaderPath("PostEffect/postEffect_vs.glsl");
		std::string shaderPath_fs = GetShaderPath("PostEffect/postEffect_fs.glsl");
		auto postEffectShader = Shader::Create(shaderPath_vs, shaderPath_fs);
		auto postEffectMat = Material::Create(postEffectShader);
		Mat_Manager.Regist("posteffect",std::move(postEffectMat));


		shaderPath_fs= GetShaderPath("PostEffect/defaultBlit_fs.glsl");
		auto defaultBlitShader = Shader::Create(shaderPath_vs, shaderPath_fs);
		auto defaultBlitMat = Material::Create(defaultBlitShader);
		Mat_Manager.Regist("defaultblit", std::move(defaultBlitMat));

		shaderPath_vs = GetShaderPath("skybox/skybox_vs.glsl");
		shaderPath_fs = GetShaderPath("skybox/skybox_fs.glsl");
		auto skybox_shader = Shader::CreateUniqueShader(shaderPath_vs, shaderPath_fs);

		// 创建立方体着色器
		std::string cubeShaderPath = GetShaderPath("cube.glsl");
		shaderPath_vs = GetShaderPath("cube_vs.glsl");
		shaderPath_fs = GetShaderPath("cube_fs.glsl");
		auto CubeShader = Shader::Create(shaderPath_vs,shaderPath_fs);
		auto CubeDefaultMat = Material::Create(CubeShader);
		CubeDefaultMat->SetTexture("u_DiffuseTexture", Texture_Manager.Get("container"), 0);
		CubeDefaultMat->SetTexture("u_MetalRoughness", Texture_Manager.Get("specular"), 1);
		CubeDefaultMat->SetFloat("roughness", 0.9);
		CubeDefaultMat->SetFloat3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
		CubeDefaultMat->SetFloat3("cameraPos_ws", glm::vec3(0.0f, 0.0f, 0.0f));
		Mat_Manager.Regist("cube", std::move(CubeDefaultMat));

		Shader_Manager.Regist("skybox", std::move(skybox_shader));


		shaderPath_fs = GetShaderPath("sphere_fs.glsl");

		auto SphereShader = Shader::Create(shaderPath_vs, shaderPath_fs);

		auto SphereDefaultMat = Material::Create(SphereShader);
		SphereDefaultMat->SetTexture("u_DiffuseTexture", Texture_Manager.Get("iron"), 0);
		SphereDefaultMat->SetTexture("u_MetalRoughness", Texture_Manager.Get("iron_metal_roughness"), 1);
		SphereDefaultMat->SetFloat("roughness", 0.9);
		SphereDefaultMat->SetFloat3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
		SphereDefaultMat->SetFloat3("cameraPos_ws", glm::vec3(0.0f, 0.0f, 0.0f));
		Mat_Manager.Regist("sphere", std::move(SphereDefaultMat));
		
	}

	void RendererLayer::SetupCube()
	{
		// 立方体顶点数据：位置(x,y,z) + 法线(nx,ny,nz,w) + uv(u,v)
		// 6 个面，每面 4 顶点，共 24 顶点
		float cubeVertices[] = {
			// 前面 (+Z) - 法线指向 +Z 方向
			-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
			// 后面 (-Z) - 法线指向 -Z 方向
			-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
			// 左面 (-X) - 法线指向 -X 方向
			-0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
			// 右面 (+X) - 法线指向 +X 方向
			 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
			// 下面 (-Y) - 法线指向 -Y 方向
			-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
			// 上面 (+Y) - 法线指向 +Y 方向
			-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		};
		unsigned int cubeIndices[] = {
			// 前面
			0, 1, 2,  2, 3, 0,
			// 后面
			4, 5, 6,  6, 7, 4,
			// 左面
			8, 9, 10,  10, 11, 8,
			// 右面
			12, 13, 14,  14, 15, 12,
			// 下面
			16, 17, 18,  18, 19, 16,
			// 上面
			20, 21, 22,  22, 23, 20
		};

		// 创建VAO
		auto CubeVAO = VertexArray::Create();
		
		// 创建VBO
		auto CubeVBO = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		
		CubeVBO->SetLayout(layout);
		CubeVAO->SetVertexBuffer(CubeVBO);

		// 创建IBO
		auto CubeIBO = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
		CubeVAO->SetIndexBuffer(CubeIBO);
		VAO_Manager.Regist("cube", std::move(CubeVAO));

	}

	void RendererLayer::SetupSphere()
	{
		struct Vertice {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			
			Vertice() = default;
			Vertice(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& norm)
				: position(pos), uv(texCoord), normal(norm) {}
			
			// 直接构造，避免临时对象
			Vertice(float x, float y, float z, float u, float v, float nx, float ny, float nz)
				: position(x, y, z), uv(u, v), normal(nx, ny, nz) {}
		};
		std::vector<Vertice>vertexs;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				vertexs.emplace_back(xPos, yPos, zPos, xSegment, ySegment, xPos, yPos, zPos);
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		// 创建VAO
		auto SphereVAO = VertexArray::Create(TopologyType::TriangleStrip);

		// 创建VBO
		auto SphereVBO = VertexBuffer::Create(vertexs.size() * sizeof(Vertice));
		SphereVBO->SetData(vertexs.data(), vertexs.size() * sizeof(Vertice));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		SphereVBO->SetLayout(layout);
		SphereVAO->SetVertexBuffer(SphereVBO);

		// 创建IBO
		auto SphereIBO = IndexBuffer::Create(indices.data(), indices.size());
		SphereVAO->SetIndexBuffer(SphereIBO);
		VAO_Manager.Regist("sphere", std::move(SphereVAO));
	}

	void RendererLayer::SetupQuad()
	{
		// 立方体顶点数据：位置(x, y, z) + 颜色(r, g, b, a)
		float quadVertices[] = {
			// 位置           颜色 (RGBA)
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		unsigned int quadIndices[] = {
			// 前面
			0,1,3,0,3,2
		};

		// 创建VAO
		auto quadVAO = VertexArray::Create();

		// 创建VBO
		auto quadVBO = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		quadVBO->SetLayout(layout);
		quadVAO->SetVertexBuffer(quadVBO);

		// 创建IBO
		auto quadIBO = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(unsigned int));
		quadVAO->SetIndexBuffer(quadIBO);
		VAO_Manager.Regist("quad", std::move(quadVAO));
	}

	void RendererLayer::SetupTexture()
	{
		std::string base_path = "resources/skybox/sea";
		std::vector<std::string>cubemap_paths = {
			(std::filesystem::path(base_path) / "right.jpg").string(),
			(std::filesystem::path(base_path) / "left.jpg").string(),
			(std::filesystem::path(base_path) / "top.jpg").string(),
			(std::filesystem::path(base_path) / "bottom.jpg").string(),
			(std::filesystem::path(base_path) / "front.jpg").string(),
			(std::filesystem::path(base_path) / "back.jpg").string()
		};
		auto skybox = TextureCube::Create(cubemap_paths);
		Texture_Manager.Regist("sea", std::move(skybox));

		auto wood_tex = Texture2D::CreateTexScope("resources/textures/wood.png", TextureFormat::SRGBA);
		Texture_Manager.Regist("wood", std::move(wood_tex));
		auto container_tex = Texture2D::CreateTexScope("resources/textures/container_box.png", TextureFormat::SRGBA,true);
		Texture_Manager.Regist("container", std::move(container_tex));
		auto specular= Texture2D::CreateTexScope("resources/textures/specular.png", TextureFormat::RED32F);
		Texture_Manager.Regist("specular", std::move(specular));
		auto iron = Texture2D::CreateTexScope("resources/textures/pbr_sphere/albedo_metal.png", TextureFormat::SRGBA,true);
		Texture_Manager.Regist("iron", std::move(iron));
		auto iron_metal_roughness = Texture2D::CreateTexScope("resources/textures/pbr_sphere/metal_roughness.png", TextureFormat::RGB8,true);
		Texture_Manager.Regist("iron_metal_roughness", std::move(iron_metal_roughness));
		auto hdr_env_house = Texture2D::CreateTexScope("resources/envmap/skybox.hdr", TextureFormat::RGB16, true,true);
		Texture_Manager.Regist("hdr_env_house", std::move(hdr_env_house));



	}

	void RendererLayer::SetUPGeoMetry()
	{
		SetupCube();
		SetupQuad();
		SetupSphere();
	}

	void RendererLayer::OnUpdate()
	{
		// 设置线框模式
		if (m_Settings.wireframe) {
			RenderCommand::SetWildFrame(true);
		} else {
			RenderCommand::SetWildFrame(false);
		}
		FBO->Bind();
		// 绑定离屏帧缓冲并设置视口
		//glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		RenderCommand::SetViewport(0, 0, GetRenderWidth(), GetRenderHeight());
		//glViewport(0, 0, (GLint)m_RTWidth, (GLint)m_RTHeight);
		 
		// 设置清屏颜色并清屏
		RenderCommand::SetClearColor(glm::vec4(0.0f,0.0f,0.0f, 1.0f));
		RenderCommand::Clear();
		FBO->ClearColorAttachments(0);

		// 开始场景
		Renderer::BeginScene();
		m_RenderPipeLine->Draw();
		Renderer::EndScene();

		// 解绑帧缓冲，恢复到默认帧缓冲，并将视口还原为窗口大小，供 ImGui 使用
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		FBO->Unbind();
		unsigned int winW = Application::Get().GetWindow().GetWidth();
		unsigned int winH = Application::Get().GetWindow().GetHeight();
		//glViewport(0, 0, (GLint)winW, (GLint)winH);
		RenderCommand::SetViewport(0, 0, (GLint)winW, (GLint)winH);

		GameTimer::Tick();

		
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
			ImGui::Checkbox("Wireframe", &m_Settings.wireframe);

			
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
				ImGui::Text("FPS: %.2f", GameTimer::GetFPB());
			ImGui::Separator();

			
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
		//m_AspectRatio = static_cast<float>(e.GetWindowWidth()) / static_cast<float>(e.GetWindowHeight());
		UpdateProjectionMatrix();
		
		// 不拦截事件，让其他层也能处理
		return false;
	}

	void RendererLayer::UpdateProjectionMatrix()
	{
		//m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
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
		
		//m_ViewMatrix = glm::lookAt(cameraPos, cameraTarget, upVector);
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
				/*ENGINE_CORE_INFO("{}Zoom", camera->Zoom);*/
				glm::mat4 viewMatrix = camera->GetViewMatrix();
				glm::mat4 projMatrix = camera->GetProjectionMatrix();
				glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

				// 绑定着色器并设置uniforms
				auto shader = renderComp.Mat->GetShader();
				shader->Bind();
				shader->SetMat4("u_Model", modelMatrix);
				shader->SetMat4("u_ViewProjection", viewProjMatrix);
				shader->SetInt("u_ObjectID", static_cast<int>(entity));
				// 绑定VAO并渲染
				renderComp.VAO->Bind();
				RenderCommand::DrawIndexed(renderComp.VAO);
			}
		}
		

	}


	void RendererLayer::ResizeRenderTarget(unsigned int width, unsigned int height)
	{
		FBO->Resize(width, height);
	}

	void RendererLayer::SetObjectIDForModel(const Ref<Model>& model, int objectID)
	{
		if (model) {
			model->SetObjectID(objectID);
		}
	}
}
