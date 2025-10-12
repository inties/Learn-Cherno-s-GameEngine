#include "pch.h"
#include "SkyBoxPass.h"

Engine::SkyBoxPass::SkyBoxPass(SkyBoxPassSpec& spec):Spec(spec)
{
	m_cubeVAO = spec.VAOManager->Get("cube").get();
	
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
}
