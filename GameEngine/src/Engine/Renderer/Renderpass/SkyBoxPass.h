#pragma once
#include "pch.h"
#include "RenderPass.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Texture.h"

namespace Engine {
	
	class SkyBoxPass :public RenderPass {
	public:
		struct SkyBoxPassSpec {
			std::string SkyBoxName;
			ResourceRegistry<VertexArray>* VAOManager;
			ResourceRegistry<Texture>* TexManager;
			ResourceRegistry<Shader>* ShaderManager;

		};
		SkyBoxPass(SkyBoxPassSpec& spec);
		void Init(RenderPipeLineSetting& pipeline_setting)override;
		void Draw()override;
	private:

		SkyBoxPassSpec Spec;
		VertexArray* m_cubeVAO;
		TextureCube* m_TextureCube;
		Texture2D* envMap;
	};
}
