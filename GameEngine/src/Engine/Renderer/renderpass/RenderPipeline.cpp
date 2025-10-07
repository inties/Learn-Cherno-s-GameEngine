#include "pch.h"
#include "RenderPipeline.h"
#include "Engine/Application.h"
#include  "Engine/Scene/Scene.h"
namespace Engine
{
	RenderPipeLine::RenderPipeLine(RenderPipeLineSetting& RenderPipeLineSetting)
	{
		// 创建渲染目标
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		ENGINE_CORE_INFO("窗口初始 x,y,{},{}", initW, initH);
		typedef TextureFormat format;
		FramebufferSpecification targetSpec = { initW,initH,{format::RGBA8,format::RED_INTEGER,format::DEPTH24STENCIL8},1 };
		RenderTarget = Framebuffer::Create(targetSpec);

		//创建离屏渲染目标
		FramebufferSpecification offscreenSpec = { initW,initH,{format::RGBA8,format::DEPTH24STENCIL8},1 };
		OffScreenTarget = Framebuffer::Create(offscreenSpec);

		//创建pass并附加渲染目标
		ForwardPass::ForwardPassSpec forwardPassSpec = { offscreenSpec,RenderPipeLineSetting.MatManager,RenderPipeLineSetting.VAOManager,RenderPipeLineSetting.scene };
		m_Forwardpass = CreateRef<ForwardPass>(forwardPassSpec);
		m_Forwardpass->SetFBO(OffScreenTarget);

		PostEffectPass::PostEffectPassSpec postpassSpec = { targetSpec,"PostEffect",RenderPipeLineSetting.MatManager };
		m_Postpass = CreateRef<PostEffectPass>(postpassSpec);
		m_Postpass->SetFBO(RenderTarget);
	}

	void RenderPipeLine::Resize() {
		RenderTarget->Resize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		OffScreenTarget->Resize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}
}
