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



	InputTexture = FBO->GetRenderTexture(0);
	FramebufferSpecification interMediateFBOspec = { InputTexture->GetHeight(),InputTexture->GetHeight(),{TextureFormat::RGBA8,TextureFormat::RGBA16},1 };
	InterMediateFBO = Framebuffer::Create(interMediateFBOspec);
	m_pipeline_settings = pipeline_setting;

	pingpong_A = Texture2D::Create(InputTexture->GetHeight(), InputTexture->GetHeight(), TextureFormat::RGBA16, 1);
	pingpong_B = Texture2D::Create(InputTexture->GetHeight(), InputTexture->GetHeight(), TextureFormat::RGBA16, 1);


}

void Engine::PostEffectPass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
{
	//离屏渲染
	InterMediateFBO->Bind();
	auto texture1 = InterMediateFBO->GetRenderTexture(0);
	texture1->Clear();
	auto texture2 = InterMediateFBO->GetRenderTexture(1);
	texture2->Clear();
	/*InterMediateFBO->ClearColorAttachments(0);*/


	//blit并提取高亮部分
	auto quadVAO = m_VAOManager->Get("quad");
	/*InputTexture = FBO->GetRenderTexture(0);*/
	auto blit_bloom_shader= m_pipeline_settings.ShaderManager->Get("blit_bloom").get();
	blit_bloom_shader->Bind();
	InputTexture->Bind(0);
	quadVAO->Bind();
	RenderCommand::DrawIndexed(quadVAO);


	RenderCommand::InsertBarrier(BarrierDomain::RenderTargetWriteToSample);

	//模糊InterMediateFBO的bloom纹理
	Bloom();


	//色调映射和gamma映射校正
	auto invert_color_shader = m_pipeline_settings.ShaderManager->Get("tonemapping_gamma").get();
	invert_color_shader->Bind();
	auto interTexture = InterMediateFBO->GetRenderTexture(0);
	auto bloomTexture = InterMediateFBO->GetRenderTexture(1);
	interTexture->Bind(0);
	bloomTexture->Bind(1);
	auto outputTexture = FBO->GetRenderTexture(0);
	ImageBindDesc output_image_desc;
	output_image_desc.binding = 1;
	output_image_desc.access = TextureAccess::WriteOnly;
	outputTexture->BindAsImage(output_image_desc);

	RenderCommand::Dispatch(std::ceil(outputTexture->GetWidth() / 8.0f), std::ceil(outputTexture->GetHeight() / 8.0f), 1);

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
	pingpong_A = Texture2D::Create(width,height, TextureFormat::RGBA16, 1);
	pingpong_B = Texture2D::Create(width,height, TextureFormat::RGBA16, 1);
}

void Engine::PostEffectPass::Bloom()
{
	auto textureA = InterMediateFBO->GetRenderTexture(1);
	auto textureB = pingpong_B;

	auto blur_shader = m_pipeline_settings.ShaderManager->Get("blur").get();
	blur_shader->Bind();
	ImageBindDesc texture_image_desc;
	texture_image_desc.binding = 0;
	texture_image_desc.access = TextureAccess::WriteOnly;

	for (int i = 0; i < 5; i++) {
		blur_shader->SetInt("vertical", 0);
		textureB->BindAsImage(texture_image_desc);
		textureA->Bind(0);
		RenderCommand::Dispatch(std::ceil(textureA->GetWidth() / 16.0f), std::ceil(textureA->GetHeight() / 16.0f), 1);
		RenderCommand::InsertBarrier(BarrierDomain::ComputeWriteToComputeRead);
		blur_shader->SetInt("vertical", 1);
		textureA->BindAsImage(texture_image_desc);
		textureB->Bind(0);
		RenderCommand::Dispatch(std::ceil(textureA->GetWidth() / 16.0f), std::ceil(textureA->GetHeight() / 16.0f), 1);
		RenderCommand::InsertBarrier(BarrierDomain::ComputeWriteToComputeRead);
	}
}


