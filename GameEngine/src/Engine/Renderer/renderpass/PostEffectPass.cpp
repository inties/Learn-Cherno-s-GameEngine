#include "pch.h"
#include "PostEffectPass.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderpass/RenderPipeline.h"
void Engine::PostEffectPass::Init(RenderPipeLineSetting& pipeline_setting)
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
	m_pipeline_settings = pipeline_setting;

}

void Engine::PostEffectPass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
{
	////离屏渲染
	InterMediateFBO->Bind();
	InterMediateFBO->ClearColorAttachments(0);
	auto quadVAO = m_VAOManager->Get("quad");
	InputTexture->Bind(0);
	PostEffectShader->Bind();
	quadVAO->Bind();
	RenderCommand::DrawIndexed(quadVAO);



	////渲染到FBO
	FBO->Bind();
	RenderCommand::Clear();
	DefaultBlitShader->Bind();
	auto InterTexture = InterMediateFBO->GetRenderTexture(0);
	InterTexture->Bind(0);
	RenderCommand::DrawIndexed(quadVAO);



}

