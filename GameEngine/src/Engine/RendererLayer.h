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
	// ��Ⱦ���ýṹ��
	struct RendererSettings
	{
		glm::vec3 clearColor{0.1f, 0.1f, 0.1f};  // ������ɫ
		float rotationSpeed = 1.0f;              // ��ת�ٶ�
		glm::vec3 position{5.0f, 0.0f, 0.0f};    // ģ��λ��
		glm::vec3 scale{1.0f, 1.0f, 1.0f};       // ģ������
		bool wireframe = false;                   // �߿�ģʽ
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

		// ���ڴ�С��������
		bool OnWindowResize(WindowResizeEvent& e);

		// ��¶���ù��ⲿ����
		RendererSettings& GetSettings() { return m_Settings; }
		const RendererSettings& GetSettings() const { return m_Settings; }

		// �ṩ��ȾĿ��������ߴ���༭��
		unsigned int GetRenderTextureID(uint32_t idx=0) const { return FBO->GetColorAttachmentRendererID(idx); }
		unsigned int GetRenderWidth() const { return FBO->GetSpecification().Width; }
		unsigned int GetRenderHeight() const { return FBO->GetSpecification().Height; }
		void ResizeRenderTarget(unsigned int width, unsigned int height);
		int ReadPickBuffer(uint32_t x, uint32_t y) { return FBO->ReadPixel(1, x, y); };

		// �ⲿ���ó���
		void SetScene(const Ref<class Scene>& scene) { m_Scene = scene; }

	private:
		// void SetupModel(); // ���Ƴ�������ʹ��Ӳ����ģ��
		void SetupCube();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();
		void LoadDefaultShaders();
		void RenderModelWithDebugShader(const glm::mat4& modelMatrix);
		void DrawRenderItems();
		// Ϊģ����������ID
		void SetObjectIDForModel(const Ref<Model>& model, int objectID);

	private:
		//// ������ȾĿ�꣨֡���壩
		//void CreateRenderTarget(unsigned int width, unsigned int height);
		//void DestroyRenderTarget();

		Ref<Framebuffer> FBO = nullptr;
	/*	unsigned int m_Framebuffer = 0;
		unsigned int m_ColorAttachment = 0;
		unsigned int m_DepthStencilRBO = 0;
		unsigned int m_RTWidth = 0;
		unsigned int m_RTHeight = 0;*/
		RendererSettings m_Settings;
		
		// ��Ⱦ��Դ
		// Ref<Model> m_Model;
		Ref<class Scene> m_Scene;
		bool m_ShowCube = true; // �Ƿ���ʾ��������ʾ
		
		// ��������Ⱦ��Դ
		Ref<VertexArray> m_CubeVAO;
		Ref<VertexBuffer> m_CubeVBO;
		Ref<IndexBuffer> m_CubeIBO;
		Ref<Shader> m_CubeShader;
		ResourceRegistry<VertexArray>VAO_Manager;
		ResourceRegistry<Material>Mat_Manager;



		// ʱ��ͱ任
		float m_Time = 0.0f;
		float m_Rotation = 0.0f;
	
		// �����ͶӰ
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		float m_AspectRatio = 1.0f;
		float m_FOV = 45.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 100.0f;
		
		// ͳ����Ϣ
		int m_FrameCount = 0;
		float m_FPS = 0.0f;
		float m_LastFPSUpdate = 0.0f;
		
		// ��������
		bool m_UseDebugShader = false;
		int m_DebugMode = 0; // 0=����, 1=λ����ɫ, 2=������ɫ, 3=��������, 4=��ɫ
		Ref<Shader> m_DebugShader;
		Ref<Model> m_model;
	};
}