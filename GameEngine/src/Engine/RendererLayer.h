#pragma once

#include "Layer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Model/Model.h"
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

	private:
		void SetupModel();
		void SetupCube();
		std::string GetShaderPath(const std::string& filename);
		void UpdateProjectionMatrix();
		void SetupViewMatrix();
		void LoadDefaultShaders();
		void RenderModelWithDebugShader(const glm::mat4& modelMatrix);

	private:
		RendererSettings m_Settings;
		
		// 渲染资源
		Ref<Model> m_Model;
		
		// 立方体渲染资源
		Ref<VertexArray> m_CubeVAO;
		Ref<VertexBuffer> m_CubeVBO;
		Ref<IndexBuffer> m_CubeIBO;
		Ref<Shader> m_CubeShader;
		
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
	};
} 