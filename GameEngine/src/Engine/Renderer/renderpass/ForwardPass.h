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
	};
	void Init(RenderPipeLineSetting& pipeline_setting)override;
	void InitEnvMapPass();
	void DrawEnvCube();
	void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)override;
	
	// 设置是否使用实例化渲染
	void SetUseInstancing(bool useInstancing) { Spec.useInstancing = useInstancing; }
	bool GetUseInstancing() const { return Spec.useInstancing; }

	private:
		void CollectRenderData();
		void RenderInstance(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*);
		// 传统逐实体渲染方法
		void DrawEntitiesIndividually();
		
		// 实例化渲染方法
		void DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*);

		ForwardPassSpec Spec;
};





}
