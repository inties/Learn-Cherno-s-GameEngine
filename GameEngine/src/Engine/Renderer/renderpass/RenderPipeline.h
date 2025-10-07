#pragma once
#include "pch.h"
#include "PostEffectPass.h"
#include "ForwardPass.h"
#include "Engine/Scene/Scene.h"
namespace Engine {
	class RenderPipeLine {
	public:
		struct RenderPipeLineSetting {
			ResourceRegistry<Material>* MatManager;
			ResourceRegistry<VertexArray>* VAOManager;
			Scene* scene;
		};
		RenderPipeLine(RenderPipeLineSetting& RenderPipeLineSetting);
		void Init() {
			m_Forwardpass->Init();
			m_Postpass->Init();
		};

		void Draw() {
			m_Forwardpass->Draw();
			m_Postpass->Draw();
		};
		void Resize();
	private:
		Ref<ForwardPass>m_Forwardpass;
		Ref<PostEffectPass>m_Postpass;
		Ref<Framebuffer>RenderTarget;
		Ref<Framebuffer>OffScreenTarget;

	};

}



