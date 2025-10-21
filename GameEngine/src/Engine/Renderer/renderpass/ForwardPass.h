#pragma once
#include "pch.h"
#include "RenderPass.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ResouceRegistry.h"
#include "Engine/Scene/Component.h"
#include "Engine/Renderer/InstancedRenderer.h"

namespace Engine {
	class ForwardPass :public RenderPass {
public:	
	struct ForwardPassSpec {
		ResourceRegistry<Material>* MatManager;
		ResourceRegistry<VertexArray>* VAOManager;
		Scene* scene;
		bool useInstancing = true;  // 是否使用实例化渲染
	};
	ForwardPass(ForwardPassSpec& spec):Spec(spec) {
		if(!spec.scene||!spec.MatManager||!spec.VAOManager){
			ENGINE_CORE_ERROR("forwardpass:Scene/MatManager/VAOManager is null");
		}
		m_InstancedRenderer = CreateScope<InstancedRenderer>();
		m_InstancedRenderer->Init();
	};
	void Init()override {};
	void InitEnvMapPass();
	void DrawEnvCube();
	void Draw()override;
	
	// 设置是否使用实例化渲染
	void SetUseInstancing(bool useInstancing) { Spec.useInstancing = useInstancing; }
	bool GetUseInstancing() const { return Spec.useInstancing; }

	private:
		// 传统逐实体渲染方法
		void DrawEntitiesIndividually();
		
		// 实例化渲染方法
		void DrawEntitiesInstanced();

	ForwardPassSpec Spec;
	Scope<InstancedRenderer> m_InstancedRenderer;
};





}
