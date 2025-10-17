#pragma once
#include "pch.h"
#include "Buffer.h"
#include "Engine/Rendererlayer.h"

namespace Engine
{
	// 实例化渲染管理器
	class InstancedRenderer
	{
	public:
		InstancedRenderer();
		~InstancedRenderer();

		// 初始化实例化渲染器
		void Init();

		// 收集实例数据
		void CollectInstances(Scene* scene, ResourceRegistry<VertexArray>* vaoManager, ResourceRegistry<Material>* matManager);

		// 执行实例化渲染
		void RenderInstanced();

		// 清理数据
		void Clear();

	private:
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
		std::unordered_map<BatchKey, BatchData,BatchKeyHash> m_Batches;
		
		// 最大实例数（必须与着色器中的数组大小匹配）
		static constexpr uint32_t MAX_INSTANCES_PER_BATCH = 10240;
		
		// SSBO绑定点
		static constexpr uint32_t INSTANCE_SSBO_BINDING = 2;
	};
}
