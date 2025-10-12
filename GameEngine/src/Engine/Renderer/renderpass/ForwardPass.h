#pragma once
#include "pch.h"
#include "RenderPass.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Scene/Component.h"
namespace Engine {
	class ForwardPass :public RenderPass {
public:	
	struct ForwardPassSpec {
		ResourceRegistry<Material>* MatManager;
		ResourceRegistry<VertexArray>* VAOManager;
		Scene* scene;
	};
	ForwardPass(ForwardPassSpec& spec):Spec(spec) {
		if(!spec.scene||!spec.MatManager||!spec.VAOManager){
			ENGINE_CORE_ERROR("forwardpass:Scene/MatManager/VAOManager is null");
		}
	};
	void Init()override {};
	void Draw()override;
	private:

	ForwardPassSpec Spec;

};





}
