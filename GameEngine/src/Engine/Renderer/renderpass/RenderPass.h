#pragma once
#include "pch.h"
#include "Engine/Renderer/FrameBuffer.h"
namespace Engine{

	class RenderPass {
	public:
		virtual void Init() = 0;
		virtual void Draw()=0;
		inline void SetFBO(Framebuffer* fbo) {
			FBO = fbo;
		}
	protected:
		Framebuffer* FBO;//FBO不由render pass管理生命周期，renderpipeline初始化时创建并分配给renderpass，允许运行时修改FBO

	};




}
