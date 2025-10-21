#include "pch.h"
#include "SkyBoxPass.h"
#include "Engine/Renderer/RenderCommand.h"
Engine::SkyBoxPass::SkyBoxPass(SkyBoxPassSpec& spec):Spec(spec)
{
	m_cubeVAO = spec.VAOManager->Get("cube").get();

	m_shader = spec.ShaderManager->Get("skybox").get();

	envMap = static_cast<Texture2D*>(spec.TexManager->Get("hdr_env_house").get());
	// 获取纹理并转换为 TextureCube*
	auto texture = spec.TexManager->Get(spec.SkyBoxName);
	if (!texture) {
		ENGINE_CORE_ERROR("SkyBoxPass: Texture '{}' not found", spec.SkyBoxName);
		m_TextureCube = nullptr;
		return;
	}
	
	m_TextureCube = dynamic_cast<TextureCube*>(texture.get());
	if (!m_TextureCube) {
		ENGINE_CORE_ERROR("SkyBoxPass: Texture '{}' is not a TextureCube", spec.SkyBoxName);
	}
}

void Engine::SkyBoxPass::Init()
{
	
}

void Engine::SkyBoxPass::Draw()
{

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	m_cubeVAO->Bind();
	//m_TextureCube->Bind(0);
	envMap->Bind(0);
	m_shader->Bind();
	MainCamera* camera = MainCamera::GetInstance();
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera->GetViewMatrix()));


	glm::mat4 projMatrix = camera->GetProjectionMatrix();
	glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
	m_shader->SetMat4("u_ViewProjection", viewProjMatrix);
	FBO->Bind();
	RenderCommand::DrawIndexed(m_cubeVAO);
	glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content
}
