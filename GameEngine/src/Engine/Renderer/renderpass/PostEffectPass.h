#pragma once
#include "pch.h"
#include "RenderPass.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Renderer/Material.h"
namespace Engine {
	class PostEffectPass :public RenderPass {
	public:
		struct PostEffectPassSpec {
			std::string matName;
			ResourceRegistry<Material>* MatManager;
		};
		PostEffectPass(PostEffectPassSpec& spec):Spec(spec) {
			if (!spec.MatManager) {
				ENGINE_CORE_ERROR("PostEffectPass:Scene/MatManager/VAOManager is null");
			}
		};
		void Init()override final{
			Mat = Spec.MatManager->Get(Spec.matName);
			if(!Mat){
				ENGINE_CORE_ERROR("PostEffectPass: Material not found");
			}
		};
		void Draw()override final{};
		inline void SetMat(Ref<Material>mat) {
			Mat = mat;
		}
	private:
		Ref<Texture>InputTexture;
		Ref<Material>Mat;
		PostEffectPassSpec Spec;
	};





}
