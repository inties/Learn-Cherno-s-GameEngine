#pragma once
#include "pch.h"
#include "RenderPass.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Scene/Component.h"

namespace Engine {
	class PostEffectPass :public RenderPass {
	public:
		struct PostEffectPassSpec {
			std::string matName;
			ResourceRegistry<Material>* MatManager;
			ResourceRegistry<VertexArray>* VAOManager;
			Scene* scene;
		};
		PostEffectPass(PostEffectPassSpec& spec) :Spec(spec) {
			if (!spec.MatManager || !spec.VAOManager || !spec.scene) {
				ENGINE_CORE_ERROR("PostEffectPass:Scene/MatManager/VAOManager is null");
			}
			m_MatManager = spec.MatManager;
			m_VAOManager = spec.VAOManager;
		};
		void Init(RenderPipeLineSetting& pipeline_setting)override final;
		void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)override final;
		void Resize(uint32_t,uint32_t);
		inline void SetMat(Ref<Material>mat) {
			Mat = mat;
		}
	private:
		void Bloom();
		
		Ref<Texture>InputTexture;
		Ref<Texture>pingpong_A;
		Ref<Texture>pingpong_B;
		Ref<Material>Mat;
		PostEffectPassSpec Spec;
		Ref<Framebuffer>InterMediateFBO;
		Ref<Shader>PostEffectShader;
		ResourceRegistry<Material>* m_MatManager;
		ResourceRegistry<VertexArray>* m_VAOManager;
	};





}
