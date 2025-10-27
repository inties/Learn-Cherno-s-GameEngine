#include "pch.h"
#include "RenderPipeline.h"
#include "Engine/Application.h"
#include  "Engine/Scene/Scene.h"
#include "Engine/Scene/Component.h"
namespace Engine
{
	RenderPipeLine::RenderPipeLine(RenderPipeLineSetting& renderPipeLineSetting)
	{
		// 创建渲染目标
		unsigned int initW = Application::Get().GetWindow().GetWidth();
		unsigned int initH = Application::Get().GetWindow().GetHeight();
		ENGINE_CORE_INFO("窗口初始 x,y,{},{}", initW, initH);
		typedef TextureFormat format;
		if (!renderPipeLineSetting.ScreenFBO) {
			ENGINE_CORE_ERROR("renderpipeline:ScreenFBO can't be null!!!");
		}
		RenderTarget = renderPipeLineSetting.ScreenFBO;

		////创建离屏渲染目标

        m_preZpass = CreateScope<Pre_Z_Pass>();
        m_preZpass->SetFBO(RenderTarget);
        m_preZpass->SetShader(renderPipeLineSetting.ShaderManager->Get("depth").get());
		//创建pass并附加渲染目标
		ForwardPass::ForwardPassSpec forwardPassSpec = {renderPipeLineSetting.MatManager,renderPipeLineSetting.VAOManager,renderPipeLineSetting.Scene };
		m_Forwardpass = CreateScope<ForwardPass>(forwardPassSpec);
		m_Forwardpass->SetFBO(RenderTarget);

		PostEffectPass::PostEffectPassSpec postpassSpec = { "posteffect",renderPipeLineSetting.MatManager,renderPipeLineSetting.VAOManager,renderPipeLineSetting.Scene };
		m_Postpass = CreateScope<PostEffectPass>(postpassSpec);
		m_Postpass->SetFBO(RenderTarget);


		SkyBoxPass::SkyBoxPassSpec skybox_spec= { "sea",renderPipeLineSetting.VAOManager,renderPipeLineSetting.TexManager,renderPipeLineSetting.ShaderManager};

		m_skyBoxPass = CreateScope<SkyBoxPass>(skybox_spec);
		m_skyBoxPass->SetFBO(RenderTarget);

		m_pipeline_setting = renderPipeLineSetting;

	}

	void RenderPipeLine::Resize(uint32_t width,uint32_t height) {
        m_Postpass->Resize(width, height);

	}

	void RenderPipeLine::CollectRenderData()
	{

           // 获取场景的registry
        auto& registry = m_pipeline_setting.Scene->GetRegistry();

        // 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
        static int i = 0;
        auto group = registry.group<RenderComponent>(entt::get<TransformComponent>);
        if (i == 0) {
            group.each([&](auto entity, auto& renderComp, auto& transComp) {
                // 绑定资源（如果还未绑定）
                if (!renderComp.IsValid()) {
                    renderComp.BindResources(*m_pipeline_setting.VAOManager, *m_pipeline_setting.MatManager);


                    // 如果资源绑定成功，添加到批次
                    if (renderComp.IsValid()) {
                        BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
                    }



                    // 创建实例数据
                    InstanceData instanceData;


                    instanceData.modelMatrix = transComp.GetTransform();
                    instanceData.extraData = glm::vec4(static_cast<float>(static_cast<int>(entity)), 0.0f, 0.0f, 0.0f);

                    // 添加到对应批次

                    BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
                    m_Batches[key].instances.push_back(instanceData);

                }

                });
        }
        i++;
        // 为每个批次创建SSBO
        for (auto& [key, batchData] : m_Batches) {
            uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());

            // 确保不超过最大实例数
            if (instanceCount > MAX_INSTANCES_PER_BATCH) {
                ENGINE_CORE_WARN("Instance count {} exceeds maximum {}, truncating", instanceCount, MAX_INSTANCES_PER_BATCH);
                instanceCount = MAX_INSTANCES_PER_BATCH;
                batchData.instances.resize(instanceCount);
            }

            // 使用固定大小SSBO（与着色器中的数组大小匹配）
            uint32_t ssboSize = MAX_INSTANCES_PER_BATCH * sizeof(InstanceData);

            // 确保SSBO容量足够
            if (!batchData.ssbo) {
                batchData.ssbo = ShaderStorageBuffer::Create(ssboSize);
                // 上传实例数据到SSBO			
            }

            // 上传实例数据到SSBO
            uint32_t dataSize = instanceCount * sizeof(InstanceData);
            batchData.ssbo->SetData(batchData.instances.data(), dataSize);
            batchData.maxInstances = instanceCount;
        }
	}

}
