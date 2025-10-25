#pragma once
#include "pch.h"
#include "PostEffectPass.h"
#include "ForwardPass.h"
#include "SkyBoxPass.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Renderpass/RenderPipelineSettings.h"
namespace Engine {
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
	class RenderPipeLine {
	public:
		RenderPipeLine(RenderPipeLineSetting& renderPipeLineSetting);
		void Init() {
			m_Forwardpass->Init(m_pipeline_setting);
			m_Postpass->Init(m_pipeline_setting);
			m_skyBoxPass->Init(m_pipeline_setting);
			m_preZpass->Init(m_pipeline_setting);
		};

		void Draw() {
			
			CollectRenderData();
			m_preZpass->Draw(&m_Batches);
			glDepthFunc(GL_LEQUAL);
			m_Forwardpass->Draw(&m_Batches);
			m_skyBoxPass->Draw(&m_Batches);
			m_Postpass->Draw(&m_Batches);
			
		};
		void Resize();
		void DrawEnvMap();
	private:
		void CollectRenderData();
	private:
		Scope<ForwardPass>m_Forwardpass;
		Scope<PostEffectPass>m_Postpass;
		Scope<SkyBoxPass>m_skyBoxPass;
		Scope<Pre_Z_Pass>m_preZpass;
		Framebuffer* RenderTarget;
		RenderPipeLineSetting m_pipeline_setting;

		// 批次映射
		std::unordered_map<BatchKey, BatchData, BatchKeyHash> m_Batches;
		// 最大实例数（必须与着色器中的数组大小匹配）
		static constexpr uint32_t MAX_INSTANCES_PER_BATCH = 10240;
	};

}



