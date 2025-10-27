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
	//离屏渲染
	InterMediateFBO->Bind();
	InterMediateFBO->ClearColorAttachments(0);
	
	auto quadVAO = m_VAOManager->Get("quad");
	InputTexture->Bind(0);
	DefaultBlitShader->Bind();
	quadVAO->Bind();
	RenderCommand::DrawIndexed(quadVAO);
	RenderCommand::InsertBarrier(BarrierDomain::RenderTargetWriteToSample);

	auto invert_color_shader = m_pipeline_settings.ShaderManager->Get("invert_color").get();
	invert_color_shader->Bind();
	auto interTexture = InterMediateFBO->GetRenderTexture(0);
	interTexture->Bind(0);
	// 2. 显式告诉 sampler uniform "u_Input 用的是单元0"
	invert_color_shader->SetInt("u_Input", 0);
	//GLint loc = glGetUniformLocation(invert_color_shader->, "u_Input");
	//glUniform1i(loc, 0);

	auto outputTexture=FBO->GetRenderTexture(0);
	ImageBindDesc output_image_desc;
	output_image_desc.binding = 1;
	output_image_desc.access = TextureAccess::WriteOnly;
	outputTexture->BindAsImage(output_image_desc);

	uint32_t width = InputTexture->GetWidth();
	uint32_t height = InputTexture->GetHeight();
	std::cout << width << " " << height << std::endl;
	std::cout << outputTexture->GetWidth()<< "   " << outputTexture->GetHeight() << std::endl;
	
	RenderCommand::Dispatch(std::ceil(outputTexture->GetWidth()/ 8), std::ceil(outputTexture->GetHeight()/ 8), 1);

	RenderCommand::InsertBarrier(BarrierDomain::ComputeWriteToRenderTarget);
	
	FBO->Bind();
	//渲染到FBO
	//FBO->Bind();
	//RenderCommand::Clear();
	//DefaultBlitShader->Bind();
	//auto InterTexture = InterMediateFBO->GetRenderTexture(0);
	//InterTexture->Bind(0);
	//RenderCommand::DrawIndexed(quadVAO);




}

void Engine::PostEffectPass::Resize(uint32_t width, uint32_t height)
{
	InterMediateFBO->Resize(width, height);
}

