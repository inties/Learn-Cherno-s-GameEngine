#pragma once

#include "Layer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "camera.h"
#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace Engine
{
	// ��Ⱦ���ýṹ��
	struct RendererSettings
	{
		glm::vec3 clearColor{0.1f, 0.1f, 0.1f};  // ������ɫ
		glm::vec3 cubeColor{1.0f, 1.0f, 1.0f};   // ��������ɫ
		float rotationSpeed = 1.0f;              // ��ת�ٶ�
		glm::vec3 position{0.0f, 0.0f, 0.0f};    // ������λ��
		glm::vec3 scale{1.0f, 1.0f, 1.0f};       // ����������
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

	private:
		void SetupCube();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();

	private:
		RendererSettings m_Settings;
		
		// ��Ⱦ��Դ
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Shader> m_Shader;
		
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

		// ����
		std::shared_ptr<Texture2D> m_CubeTexture;
	};
} 