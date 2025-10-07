#pragma once
#include "pch.h"
#include "RenderPass.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Renderer/Material.h"
namespace Engine {
	class PostEffectPass :public RenderPass {
	public:
		struct PostEffectPassSpec {
			FramebufferSpecification frameBufferSpecfication;
			std::string matName;
			ResourceRegistry<Material>* matLib;
		};
		PostEffectPass(PostEffectPassSpec& spec):Spec(spec) {};
		void Init()override final{
			Mat = Spec.matLib->Get(Spec.matName);
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
