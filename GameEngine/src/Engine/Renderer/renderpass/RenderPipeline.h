#pragma once
#include "pch.h"
#include "PostEffectPass.h"
#include "ForwardPass.h"
#include "SkyBoxPass.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Renderpass/RenderPipelineSettings.h"
namespace Engine {
	class RenderPipeLine {
	public:
		RenderPipeLine(RenderPipeLineSetting& renderPipeLineSetting);
		void Init() {
			m_Forwardpass->Init(m_pipeline_setting);
			m_Postpass->Init(m_pipeline_setting);
			m_skyBoxPass->Init(m_pipeline_setting);
		};

		void Draw() {
			m_Forwardpass->Draw();
			m_skyBoxPass->Draw();
			m_Postpass->Draw();
		};
		void Resize();
		void DrawEnvMap();
	private:
		Scope<ForwardPass>m_Forwardpass;
		Scope<PostEffectPass>m_Postpass;
		Scope<SkyBoxPass>m_skyBoxPass;
		Framebuffer* RenderTarget;
		RenderPipeLineSetting m_pipeline_setting;
		//Ref<Framebuffer>OffScreenTarget;

	};

}



