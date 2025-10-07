#pragma once
#include "pch.h"
#include "Layer.h"
#include "Renderer/Renderer.h"
#include "Resources/ResouceRegistry.h"
#include "Model/Model.h"
#include "camera.h"
#include "Application.h"
#include "Scene/Entity.h"


namespace Engine
{
	// 渲染设置结构体
	struct RendererSettings
	{
		glm::vec3 clearColor{0.1f, 0.1f, 0.1f};  // 清屏颜色
		float rotationSpeed = 1.0f;              // 旋转速度
		glm::vec3 position{5.0f, 0.0f, 0.0f};    // 模型位置
		glm::vec3 scale{1.0f, 1.0f, 1.0f};       // 模型缩放
		bool wireframe = false;                   // 线框模式
	};

	class ENGINE_API RendererLayer : public Layer
	{
	public:
		RendererLayer();
		virtual ~RendererLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		// 窗口大小调整处理
		bool OnWindowResize(WindowResizeEvent& e);

		// 暴露设置供外部访问
		RendererSettings& GetSettings() { return m_Settings; }
		const RendererSettings& GetSettings() const { return m_Settings; }

		// 提供渲染目标纹理与尺寸给编辑器
		unsigned int GetRenderTextureID(uint32_t idx=0) const { return FBO->GetColorAttachmentRendererID(idx); }
		unsigned int GetRenderWidth() const { return FBO->GetSpecification().Width; }
		unsigned int GetRenderHeight() const { return FBO->GetSpecification().Height; }
		void ResizeRenderTarget(unsigned int width, unsigned int height);
		int ReadPickBuffer(uint32_t x, uint32_t y) { return FBO->ReadPixel(1, x, y); };

		// 外部设置场景
		void SetScene(const Ref<class Scene>& scene) { m_Scene = scene; }

	private:
		// void SetupModel(); // 已移除：不再使用硬编码模型
		void SetupCube();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();
		void LoadDefaultShaders();
		void RenderModelWithDebugShader(const glm::mat4& modelMatrix);
		void DrawRenderItems();
		// 为模型设置物体ID
		void SetObjectIDForModel(const Ref<Model>& model, int objectID);

	private:
		//// 离屏渲染目标（帧缓冲）
		//void CreateRenderTarget(unsigned int width, unsigned int height);
		//void DestroyRenderTarget();

		Ref<Framebuffer> FBO = nullptr;
	/*	unsigned int m_Framebuffer = 0;
		unsigned int m_ColorAttachment = 0;
		unsigned int m_DepthStencilRBO = 0;
		unsigned int m_RTWidth = 0;
		unsigned int m_RTHeight = 0;*/
		RendererSettings m_Settings;
		
		// 渲染资源
		// Ref<Model> m_Model;
		Ref<class Scene> m_Scene;
		bool m_ShowCube = true; // 是否显示立方体演示
		
		// 立方体渲染资源
		Ref<VertexArray> m_CubeVAO;
		Ref<VertexBuffer> m_CubeVBO;
		Ref<IndexBuffer> m_CubeIBO;
		Ref<Shader> m_CubeShader;
		ResourceRegistry<VertexArray>VAO_Manager;
		ResourceRegistry<Material>Mat_Manager;



		// 时间和变换
		float m_Time = 0.0f;
		float m_Rotation = 0.0f;
	
		// 相机和投影
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		float m_AspectRatio = 1.0f;
		float m_FOV = 45.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 100.0f;
		
		// 统计信息
		int m_FrameCount = 0;
		float m_FPS = 0.0f;
		float m_LastFPSUpdate = 0.0f;
		
		// 调试设置
		bool m_UseDebugShader = false;
		int m_DebugMode = 0; // 0=正常, 1=位置颜色, 2=法线颜色, 3=纹理坐标, 4=纯色
		Ref<Shader> m_DebugShader;
		Ref<Model> m_model;
	};
}