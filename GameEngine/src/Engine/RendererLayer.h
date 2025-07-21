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
	// 渲染设置结构体
	struct RendererSettings
	{
		glm::vec3 clearColor{0.1f, 0.1f, 0.1f};  // 清屏颜色
		glm::vec3 cubeColor{1.0f, 1.0f, 1.0f};   // 立方体颜色
		float rotationSpeed = 1.0f;              // 旋转速度
		glm::vec3 position{0.0f, 0.0f, 0.0f};    // 立方体位置
		glm::vec3 scale{1.0f, 1.0f, 1.0f};       // 立方体缩放
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

	private:
		void SetupCube();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();

	private:
		RendererSettings m_Settings;
		
		// 渲染资源
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Shader> m_Shader;
		
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

		// 纹理
		std::shared_ptr<Texture2D> m_CubeTexture;
	};
} 