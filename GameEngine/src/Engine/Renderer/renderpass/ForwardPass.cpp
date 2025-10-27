#include "pch.h"
#include "ForwardPass.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/camera.h"
#include "Engine/Scene/Light.h"
#include "Engine/Renderer/Renderpass/RenderPipeline.h"
namespace Engine {
    void ForwardPass::Init(RenderPipeLineSetting& pipeline_setting)
    {
        m_pipeline_settings = pipeline_setting;

    }
    void ForwardPass::InitEnvMapPass() {
     
    }
    void ForwardPass::DrawEnvCube()
    {
        
    }
    void ForwardPass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data) {
        // 使用ECS系统渲染有RenderComponent的实体
        if (!Spec.scene || !Spec.VAOManager || !Spec.MatManager) {
            ENGINE_CORE_ERROR("forwardpass:Scene/VAOmanager/MatManager为空");
            return;
        }

        // 根据设置选择渲染方式
        if (Spec.useInstancing) {
            DrawEntitiesInstanced(batch_data);
        } else {
            DrawEntitiesIndividually();
        }
    }

    void ForwardPass::CollectRenderData()
    { 
            // 清空之前的批次
             //Clear();

            // 获取场景的registry
            //auto& registry = m_pipeline_settings.Scene->GetRegistry();

            //// 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
            //static int i = 0;
            //auto group = registry.group<RenderComponent>(entt::get<TransformComponent>);
            //if (i == 0) {
            //    group.each([&](auto entity, auto& renderComp, auto& transComp) {
            //        // 绑定资源（如果还未绑定）
            //        if (!renderComp.IsValid()) {
            //            renderComp.BindResources(*m_pipeline_settings.VAOManager, *m_pipeline_settings.MatManager);


            //            // 如果资源绑定成功，添加到批次
            //            if (renderComp.IsValid()) {
            //                BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
            //            }



            //            // 创建实例数据
            //            InstanceData instanceData;


            //            instanceData.modelMatrix = transComp.GetTransform();
            //            instanceData.extraData = glm::vec4(static_cast<float>(static_cast<int>(entity)), 0.0f, 0.0f, 0.0f);

            //            // 添加到对应批次

            //            BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
            //            m_Batches[key].instances.push_back(instanceData);

            //        }

            //        });
            //}
            //i++;
            //// 为每个批次创建SSBO
            //for (auto& [key, batchData] : m_Batches) {
            //    uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());

            //    // 确保不超过最大实例数
            //    if (instanceCount > MAX_INSTANCES_PER_BATCH) {
            //        ENGINE_CORE_WARN("Instance count {} exceeds maximum {}, truncating", instanceCount, MAX_INSTANCES_PER_BATCH);
            //        instanceCount = MAX_INSTANCES_PER_BATCH;
            //        batchData.instances.resize(instanceCount);
            //    }

            //    // 使用固定大小SSBO（与着色器中的数组大小匹配）
            //    uint32_t ssboSize = MAX_INSTANCES_PER_BATCH * sizeof(InstanceData);

            //    // 确保SSBO容量足够
            //    if (!batchData.ssbo) {
            //        batchData.ssbo = ShaderStorageBuffer::Create(ssboSize);
            //        // 上传实例数据到SSBO			
            //    }

            //    // 上传实例数据到SSBO
            //    uint32_t dataSize = instanceCount * sizeof(InstanceData);
            //    batchData.ssbo->SetData(batchData.instances.data(), dataSize);
            //    batchData.maxInstances = instanceCount;
            //}    
        
    }



    void ForwardPass::DrawEntitiesIndividually() {
        // 获取场景的registry
        auto& registry = Spec.scene->GetRegistry();

        // 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
        auto view = registry.view<RenderComponent, TransformComponent>();

        for (auto& [entity, renderComp, TransComp] : view.each()) {
            // 绑定资源（如果还未绑定）
            if (!renderComp.IsValid()) {
                renderComp.BindResources(*(Spec.VAOManager), *(Spec.MatManager));
            }

            // 如果资源绑定成功，进行渲染
            if (renderComp.IsValid()) {
                // 创建变换矩阵
                glm::mat4 modelMatrix = TransComp.GetTransform();

                // 获取视图矩阵
                MainCamera* camera = MainCamera::GetInstance();
                glm::mat4 viewMatrix = camera->GetViewMatrix();
                glm::mat4 projMatrix = camera->GetProjectionMatrix();

         
                


                glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
                
                renderComp.Mat->Bind();
                // 绑定着色器并设置uniforms
                auto shader = renderComp.Mat->GetShader();
                shader->Bind();
                shader->SetMat4("u_Model", modelMatrix);
                shader->SetMat4("u_ViewProjection", viewProjMatrix);
                shader->SetInt("u_ObjectID", static_cast<int>(entity));
                // 绑定VAO并渲染
                renderComp.VAO->Bind();
                RenderCommand::DrawIndexed(renderComp.VAO);
            }
        }
    }

    void ForwardPass::DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data) { 
        // 获取相机矩阵
        MainCamera* camera = MainCamera::GetInstance();
        if (!camera) {
            ENGINE_CORE_ERROR("No camera available for instanced rendering");
            return;
        }

        glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projMatrix = camera->GetProjectionMatrix();
        
        glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
               // 打印投影矩阵
        //// 渲染每个批次
        for (auto& [key, batchData] : *batch_data) {
            if (batchData.instances.empty()) continue;

            //    // 绑定材质和着色器
            key.material->Bind();
            auto shader = key.material->GetShader();
            shader->Bind();

            // 设置视图投影矩阵
            shader->SetMat4("u_ViewProjection", viewProjMatrix);

            // 使用MainLight单例设置光源
            auto& mainLight = MainLight::GetInstance();
            shader->SetFloat3("direct_light_dir", mainLight.GetDirection());
            shader->SetFloat3("direct_light_strength", mainLight.GetStrength());
            shader->SetFloat3("cameraPos_ws", camera->GetPosition());
            // 绑定SSBO
            m_pipeline_settings.lights_gpu->Bind(1);
            batchData.ssbo->Bind(2);
            // 绑定VAO
            key.vao->Bind();

            //    // 执行实例化绘制
            uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
            RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
        }

    }

};

