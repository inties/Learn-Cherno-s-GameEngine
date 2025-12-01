#include "pch.h"
#include "RenderPipeline.h"
#include "Engine/Application.h"
#include  "Engine/Scene/Scene.h"
#include "Engine/Scene/Component.h"
#include "Engine/RendererLayer.h"
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
        
        FramebufferSpecification spec = { initW,initH,{format::RGBA16,format::RED_INTEGER,format::RED32F,format::DEPTH24STENCIL8},1 };
        m_transparent_pass_RT = Framebuffer::CreateScope(spec);
        
        
		////创建离屏渲染目标

        m_preZpass = CreateScope<Pre_Z_Pass>();
        m_preZpass->SetFBO(RenderTarget);
        m_preZpass->SetShader(renderPipeLineSetting.ShaderManager->Get("depth").get());
		//创建pass并附加渲染目标
		
		m_opaque_pass = CreateScope<OpaqueForwardPass>();
		m_opaque_pass->SetFBO(RenderTarget);

        m_transparent_pass = CreateScope<TransparentForwardPass>();
        m_transparent_pass->SetFBO(m_transparent_pass_RT.get());

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
        m_preZpass->Resize(width, height);
        m_transparent_pass_RT->Resize(width, height);

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
                    m_Batches[(int)renderComp.renderlayer][key].instances.push_back(instanceData);

                }

                });
        }
        i++;
        // 只为Transparent和opaque创建SSBO
        for (int i = 0; i <= (int)RenderItemLayer::Transparent; i++) {
            for (auto& [key, batchData] : m_Batches[i]) {
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

    //UI绘制，暂时只绘制光源代理网格
    void RenderPipeLine::DrawUI()
    {
        for (auto& [key, batchData] : m_Batches[(int)RenderItemLayer::Other]) {
            m_pipeline_setting.lights_gpu->Bind(2);
            key.material->Bind();
            MainCamera* camera = MainCamera::GetInstance();
            if (!camera) {
                ENGINE_CORE_ERROR("No camera available for instanced rendering");
                return;
            }

            glm::mat4 viewMatrix = camera->GetViewMatrix();
            glm::mat4 projMatrix = camera->GetProjectionMatrix();

            glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
            // 设置视图投影矩阵
            key.material->SetMat4("u_ViewProjection", viewProjMatrix);
            // 绑定VAO
            key.vao->Bind();

            // 执行实例化绘制
            uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
            RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
        }
    }

}
