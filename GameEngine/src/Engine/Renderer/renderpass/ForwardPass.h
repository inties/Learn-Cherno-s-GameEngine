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
	void Draw()override;
	
	// 设置是否使用实例化渲染
	void SetUseInstancing(bool useInstancing) { Spec.useInstancing = useInstancing; }
	bool GetUseInstancing() const { return Spec.useInstancing; }

	private:
		void CollectRenderData();
		void RenderInstance();
		// 传统逐实体渲染方法
		void DrawEntitiesIndividually();
		
		// 实例化渲染方法
		void DrawEntitiesInstanced();
		// 批次键结构
		struct BatchKey {
			VertexArray* vao;
			Material* material;

			bool operator==(const BatchKey& other) const {
				return vao == other.vao && material == other.material;
			}
		};

		// 批次键哈希函数
		struct BatchKeyHash {
			std::size_t operator()(const BatchKey& key) const {
				return std::hash<void*>{}(key.vao) ^ (std::hash<void*>{}(key.material) << 1);
			}
		};

		// 批次数据
		struct BatchData {
			std::vector<InstanceData> instances;
			Scope<ShaderStorageBuffer> ssbo;
			uint32_t maxInstances;
		};

		// 批次映射
		std::unordered_map<BatchKey, BatchData, BatchKeyHash> m_Batches;

		// 最大实例数（必须与着色器中的数组大小匹配）
		static constexpr uint32_t MAX_INSTANCES_PER_BATCH = 10240;
		ForwardPassSpec Spec;
};





}
