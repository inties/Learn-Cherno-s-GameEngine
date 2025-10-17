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
	class Scene;
	class RenderPipeLine;
	// 实例化数据结构体 - 确保std140对齐
	struct InstanceData {
		glm::mat4 modelMatrix;    // 64 bytes (16 * 4)
		glm::vec4 extraData;      // 16 bytes (x: objectID, yzw: 自定义数据)
		
		InstanceData() = default;
		InstanceData(const glm::mat4& model, int objectId) 
			: modelMatrix(model), extraData(objectId, 0.0f, 0.0f, 0.0f) {}
	};
	// 渲染设置结构体
	struct RendererSettings
	{
		bool wireframe = false;                   // 线框模式
	};

	class ENGINE_API RendererLayer : public Layer
	{
	public:
		RendererLayer();
		virtual ~RendererLayer();
		void Init(Scene* scene);
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
		void SetScene(Scene* scene) { m_Scene = scene; }

	private:
		// void SetupModel(); // 已移除：不再使用硬编码模型
		void SetUPGeoMetry();
		void SetupCube();
		void SetupQuad();
		void SetupTexture();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();
		void SetUpShadersMaterials();
		void RenderModelWithDebugShader(const glm::mat4& modelMatrix);
		void DrawRenderItems();
		// 为模型设置物体ID
		void SetObjectIDForModel(const Ref<Model>& model, int objectID);

	private:
		std::unordered_map<Material*, std::unordered_map<VertexArray*, std::vector<InstanceData>>>renderer_resource;
		Scope<RenderPipeLine>m_RenderPipeLine;
		Ref<Framebuffer> FBO = nullptr;
		RendererSettings m_Settings;
		
		// 渲染资源
		// Ref<Model> m_Model;
		Scene* m_Scene;

		
		// 立方体渲染资源
		Ref<VertexArray> m_CubeVAO;
		Ref<VertexBuffer> m_CubeVBO;
		Ref<IndexBuffer> m_CubeIBO;
		Ref<Shader> m_CubeShader;
		ResourceRegistry<VertexArray>VAO_Manager;
		ResourceRegistry<Material>Mat_Manager;
		ResourceRegistry<Texture>Texture_Manager;
		ResourceRegistry<Shader>Shader_Manager;
		


		// 时间和变换
		float m_Time = 0.0f;

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
