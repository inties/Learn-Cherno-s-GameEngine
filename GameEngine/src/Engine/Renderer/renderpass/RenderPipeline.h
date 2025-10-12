#pragma once
#include "pch.h"
#include "PostEffectPass.h"
#include "ForwardPass.h"
#include "SkyBoxPass.h"
#include "Engine/Scene/Scene.h"
namespace Engine {
	struct RenderPipeLineSetting {
		ResourceRegistry<Material>* MatManager;
		ResourceRegistry<VertexArray>* VAOManager;
		ResourceRegistry<Texture>* TexManager;
		ResourceRegistry<Shader>* ShaderManager;
		Framebuffer* ScreenFBO;
		Scene* Scene;
	};
	class RenderPipeLine {
	public:
		RenderPipeLine(RenderPipeLineSetting& renderPipeLineSetting);
		void Init() {
			m_Forwardpass->Init();
			m_Postpass->Init();
			m_skyBoxPass->Init();
		};

		void Draw() {
			m_Forwardpass->Draw();
			m_skyBoxPass->Draw();
			m_Postpass->Draw();
		};
		void Resize();
	private:
		Scope<ForwardPass>m_Forwardpass;
		Scope<PostEffectPass>m_Postpass;
		Scope<SkyBoxPass>m_skyBoxPass;
		Framebuffer* RenderTarget;
		//Ref<Framebuffer>OffScreenTarget;

	};

}



