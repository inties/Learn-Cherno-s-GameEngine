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
	//struct ForwardPassSpec {
	//	ResourceRegistry<Material>* MatManager;
	//	ResourceRegistry<VertexArray>* VAOManager;
	//	Scene* scene;
	//	bool useInstancing = true;  // 是否使用实例化渲染
	//};
	ForwardPass() = default;
	void Init(RenderPipeLineSetting& pipeline_setting)override;
	void InitEnvMapPass();
	void DrawEnvCube();
	void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)override;
	// 设置是否使用实例化渲染
	void SetUseInstancing(bool useInstancing) { m_useInstancing = useInstancing; }
	bool GetUseInstancing() const { return m_useInstancing; }

	
	void GetCulledLights(ShaderStorageBuffer* culled_lights) {
		visible_lights_id = culled_lights;
	};
	

	protected:
		bool m_useInstancing = true;
		// 传统逐实体渲染方法
		virtual void DrawEntitiesIndividually();
		
		// 实例化渲染方法
		virtual void DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*);


		ShaderStorageBuffer* visible_lights_id;
};
	class OpaqueForwardPass :public ForwardPass {
	public:
		void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)override;
	protected:
		void DrawEntitiesIndividually()override;
		void DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*) override;
	};
	class TransparentForwardPass :public ForwardPass {
	public:
		void Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)override;
	protected:
		void DrawEntitiesIndividually()override;
		void DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*) override;
	};



}
