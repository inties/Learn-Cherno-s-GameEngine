#pragma once
#include "pch.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/camera.h"
#include "RenderPipelineSettings.h"

namespace Engine{
	struct BatchKey;
	struct BatchData;
	struct BatchKeyHash;
	class RenderPass {
	public:
		RenderPass() = default;
		virtual void Init(RenderPipeLineSetting&) = 0;
		virtual void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*)=0;
		inline void SetFBO(Framebuffer* fbo) {
			FBO = fbo;
		}
		inline void SetShader(Shader* shader) {
			m_shader = shader;
		}
	protected:
		Framebuffer* FBO;//FBO不由render pass管理生命周期，renderpipeline初始化时创建并分配给renderpass，允许运行时修改FBO
		Shader* m_shader;
		RenderPipeLineSetting m_pipeline_settings;
	};
	class Pre_Z_Pass :public RenderPass {
		void Init(RenderPipeLineSetting& pipeline_setting) {};
		void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*batch_data)override {
			//绑定深度缓冲
			//绑定VAO
			//绘制调用
		};
		inline void SetFBO(Framebuffer* fbo) {
			FBO = fbo;
		}
		inline void SetShader(Shader* shader) {
			m_shader = shader;
		}
		inline void SetVAO(VertexArray* vao) {
			m_VAO = vao;
		}
	protected:
		Framebuffer* FBO;//FBO不由render pass管理生命周期，renderpipeline初始化时创建并分配给renderpass，允许运行时修改FBO
		Shader* m_shader;
		VertexArray* m_VAO;
		ShaderStorageBuffer* m_SSBO;
		uint32_t m_instanceCount;
		RenderPipeLineSetting m_pipeline_settings;
	};



}
