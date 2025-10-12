#include "pch.h"
#include "RenderPipeline.h"
#include "Engine/Application.h"
#include  "Engine/Scene/Scene.h"
namespace Engine
{
	RenderPipeLine::RenderPipeLine(RenderPipeLineSetting& renderPipeLineSetting)
	{
		// 创建渲染目标
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		ENGINE_CORE_INFO("窗口初始 x,y,{},{}", initW, initH);
		typedef TextureFormat format;
		if (!renderPipeLineSetting.ScreenFBO) {
			ENGINE_CORE_ERROR("renderpipeline:ScreenFBO can't be null!!!");
		}
		RenderTarget = renderPipeLineSetting.ScreenFBO;

		////创建离屏渲染目标
		//FramebufferSpecification offscreenSpec = { initW,initH,{format::RGBA8,format::DEPTH24STENCIL8},1 };
		//OffScreenTarget = Framebuffer::Create(offscreenSpec);

		//创建pass并附加渲染目标
		ForwardPass::ForwardPassSpec forwardPassSpec = {renderPipeLineSetting.MatManager,renderPipeLineSetting.VAOManager,renderPipeLineSetting.Scene };
		m_Forwardpass = CreateScope<ForwardPass>(forwardPassSpec);
		m_Forwardpass->SetFBO(RenderTarget);

		PostEffectPass::PostEffectPassSpec postpassSpec = { "posteffect",renderPipeLineSetting.MatManager,renderPipeLineSetting.VAOManager,renderPipeLineSetting.Scene };
		m_Postpass = CreateScope<PostEffectPass>(postpassSpec);
		m_Postpass->SetFBO(RenderTarget);


		SkyBoxPass::SkyBoxPassSpec sky_box_pass_spec= { "sea",renderPipeLineSetting.VAOManager,renderPipeLineSetting.TexManager};

		m_Postpass = CreateScope<PostEffectPass>(postpassSpec);
		//m_Postpass->SetShader();
		m_Postpass->SetFBO(RenderTarget);

	}

	void RenderPipeLine::Resize() {
		RenderTarget->Resize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		//OffScreenTarget->Resize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}
}
