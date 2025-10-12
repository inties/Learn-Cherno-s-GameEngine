#pragma once
#include "pch.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/camera.h"
namespace Engine{
	class RenderPass {
	public:
		virtual void Init() = 0;
		virtual void Draw()=0;
		inline void SetFBO(Framebuffer* fbo) {
			FBO = fbo;
		}
		inline void SetShader(Shader* shader) {
			m_shader = shader;
		}
	protected:
		Framebuffer* FBO;//FBO不由render pass管理生命周期，renderpipeline初始化时创建并分配给renderpass，允许运行时修改FBO
		Shader* m_shader;
	};




}
