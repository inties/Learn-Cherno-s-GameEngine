#include "pch.h"
#include "PostEffectPass.h"
#include "Engine/Renderer/RenderCommand.h"
void Engine::PostEffectPass::Init()
{
	Mat = Spec.MatManager->Get(Spec.matName);
	if (!Mat) {
		ENGINE_CORE_ERROR("PostEffectPass: Material {} not found", Spec.matName);
	}
	PostEffectShader = Mat->GetShader();

	DefaultBlitShader = Spec.MatManager->Get("defaultblit")->GetShader();

	InputTexture = FBO->GetRenderTexture(0);
	FramebufferSpecification interMediateFBOspec = { InputTexture->GetHeight(),InputTexture->GetHeight(),{TextureFormat::RGBA8},1 };
	InterMediateFBO = Framebuffer::Create(interMediateFBOspec);
}

void Engine::PostEffectPass::Draw()
{
	//离屏渲染
	InterMediateFBO->Bind();
	InterMediateFBO->ClearColorAttachments(0);
	auto quadVAO = m_VAOManager->Get("quad");
	InputTexture->Bind(0);
	PostEffectShader->Bind();
	quadVAO->Bind();
	RenderCommand::DrawIndexed(quadVAO);



	//渲染到FBO
	FBO->Bind();
	RenderCommand::Clear();
	DefaultBlitShader->Bind();
	auto InterTexture = InterMediateFBO->GetRenderTexture(0);
	InterTexture->Bind(0);
	RenderCommand::DrawIndexed(quadVAO);



}

