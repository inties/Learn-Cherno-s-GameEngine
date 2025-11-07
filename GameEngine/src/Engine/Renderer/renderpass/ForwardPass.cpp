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
        if (!m_pipeline_settings.Scene || !m_pipeline_settings.VAOManager || !m_pipeline_settings.MatManager) {
            ENGINE_CORE_ERROR("forwardpass:Scene/VAOmanager/MatManager为空");
            return;
        }

        // 根据设置选择渲染方式
        if (m_useInstancing) {
            DrawEntitiesInstanced(batch_data);
        } else {
            DrawEntitiesIndividually();
        }
    }

  



    void ForwardPass::DrawEntitiesIndividually() {
        //// 获取场景的registry
        //auto& registry = Spec.scene->GetRegistry();

        //// 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
        //auto view = registry.view<RenderComponent, TransformComponent>();

        //for (auto& [entity, renderComp, TransComp] : view.each()) {
        //    // 绑定资源（如果还未绑定）
        //    if (!renderComp.IsValid()) {
        //        renderComp.BindResources(*(Spec.VAOManager), *(Spec.MatManager));
        //    }

        //    // 如果资源绑定成功，进行渲染
        //    if (renderComp.IsValid()) {
        //        // 创建变换矩阵
        //        glm::mat4 modelMatrix = TransComp.GetTransform();

        //        // 获取视图矩阵
        //        MainCamera* camera = MainCamera::GetInstance();
        //        glm::mat4 viewMatrix = camera->GetViewMatrix();
        //        glm::mat4 projMatrix = camera->GetProjectionMatrix();

        // 
        //        


        //        glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
        //        
        //        renderComp.Mat->Bind();
        //        // 绑定着色器并设置uniforms
        //        auto shader = renderComp.Mat->GetShader();
        //        shader->Bind();
        //        shader->SetMat4("u_Model", modelMatrix);
        //        shader->SetMat4("u_ViewProjection", viewProjMatrix);
        //        shader->SetInt("u_ObjectID", static_cast<int>(entity));
        //        // 绑定VAO并渲染
        //        renderComp.VAO->Bind();
        //        RenderCommand::DrawIndexed(renderComp.VAO);
        //    }
        //}
    }

    void ForwardPass::DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data) { 
       // // 获取相机矩阵
       // MainCamera* camera = MainCamera::GetInstance();
       // if (!camera) {
       //     ENGINE_CORE_ERROR("No camera available for instanced rendering");
       //     return;
       // }

       // glm::mat4 viewMatrix = camera->GetViewMatrix();
       // glm::mat4 projMatrix = camera->GetProjectionMatrix();
       // 
       // glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

       ////不透明物体渲染
       // for (auto& [key, batchData] : batch_data[(int)RenderItemLayer::Opaque]) {
       //     if (batchData.instances.empty()) continue;

       //     //// 绑定材质和着色器
       //     key.material->Bind();
       //     auto shader = key.material->GetShader();
       //     shader->Bind();

       //     // 设置视图投影矩阵
       //     shader->SetMat4("u_ViewProjection", viewProjMatrix);

       //     // 使用MainLight单例设置光源
       //     auto& mainLight = MainLight::GetInstance();
       //     shader->SetFloat3("direct_light_dir", mainLight.GetDirection());
       //     shader->SetFloat3("direct_light_strength", mainLight.GetStrength());
       //     shader->SetFloat3("cameraPos_ws", camera->GetPosition());
       //     shader->SetFloat2("screen_size", glm::vec2(FBO->GetRenderTexture(0)->GetWidth(), FBO->GetRenderTexture(0)->GetHeight()));
       //     // 设置光源
       //     m_pipeline_settings.lights_gpu->Bind(1);
       //     visible_lights_id->Bind(3);
       //     batchData.ssbo->Bind(2);
       //     // 绑定VAO
       //     key.vao->Bind();

       //     // 执行实例化绘制
       //     uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
       //     RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
       // }

       // //设置混合参数
       // BlendDesc blend_desc;
       // blend_desc.independentBlendEnable = true;
       // blend_desc.renderTarget[0] = {
       //         true,  // blendEnable
       //         BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add,
       //         BlendFactor::One, BlendFactor::Zero, BlendOp::Add
       // };
       // blend_desc.renderTarget[1] = {
       //         false,  // blendEnable
       //         BlendFactor::One, BlendFactor::Zero, BlendOp::Add,
       //         BlendFactor::One, BlendFactor::Zero, BlendOp::Add
       // };
       // RenderCommand::SetBlendState(blend_desc);

       // //仅深度测试，不深度写入
       // RenderCommand::SetDepthStencilState(DepthStencilDesc::ReadOnly());
       // //半透明物体绘制
       // for (auto& [key, batchData] : batch_data[(int)RenderItemLayer::Transparent]) {
       //     if (batchData.instances.empty()) continue;

       //     // 绑定材质和着色器
       //     key.material->Bind();
       //     auto shader = key.material->GetShader();
       //     shader->Bind();

       //     // 设置视图投影矩阵
       //     shader->SetMat4("u_ViewProjection", viewProjMatrix);

       //     // 使用MainLight单例设置光源
       //     auto& mainLight = MainLight::GetInstance();
       //     shader->SetFloat3("direct_light_dir", mainLight.GetDirection());
       //     shader->SetFloat3("direct_light_strength", mainLight.GetStrength());
       //     shader->SetFloat3("cameraPos_ws", camera->GetPosition());
       //     shader->SetFloat2("screen_size", glm::vec2(FBO->GetRenderTexture(0)->GetWidth(), FBO->GetRenderTexture(0)->GetHeight()));
       //     // 设置光源
       //     m_pipeline_settings.lights_gpu->Bind(1);
       //     visible_lights_id->Bind(3);
       //     batchData.ssbo->Bind(2);
       //     // 绑定VAO
       //     key.vao->Bind();

       //     // 执行实例化绘制
       //     uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
       //     RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
       // }
       // //重新设置为不透明绘制
       // auto opaque=BlendDesc::CreateOpaque();
       // RenderCommand::SetBlendState(opaque);
       // RenderCommand::SetDepthStencilState(DepthStencilDesc::Default());

       

    }

    void OpaqueForwardPass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
    {
        // 使用ECS系统渲染有RenderComponent的实体
        if (!m_pipeline_settings.Scene || !m_pipeline_settings.VAOManager || !m_pipeline_settings.MatManager) {
            ENGINE_CORE_ERROR("forwardpass:Scene/VAOmanager/MatManager为空");
            return;
        }

        // 根据设置选择渲染方式
        if (m_useInstancing) {
            DrawEntitiesInstanced(batch_data);
        }
        else {
            DrawEntitiesIndividually();
        }
    }

    void OpaqueForwardPass::DrawEntitiesIndividually()
    {
    }

    void OpaqueForwardPass::DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*batch_data)
    {
        DepthStencilDesc depthdesc = DepthStencilDesc::Default();
        depthdesc.depthCompare = CompareOp::LessEqual;
        RenderCommand::SetDepthStencilState(depthdesc);
        // 获取相机矩阵
        MainCamera* camera = MainCamera::GetInstance();
        if (!camera) {
            ENGINE_CORE_ERROR("No camera available for instanced rendering");
            return;
        }

        glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projMatrix = camera->GetProjectionMatrix();

        glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

        //不透明物体渲染
        for (auto& [key, batchData] : batch_data[(int)RenderItemLayer::Opaque]) {
            if (batchData.instances.empty()) continue;

            //// 绑定材质和着色器
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
            shader->SetFloat2("screen_size", glm::vec2(FBO->GetRenderTexture(0)->GetWidth(), FBO->GetRenderTexture(0)->GetHeight()));
            shader->SetFloat("roughness", 0.5f);
            shader->SetInt("transparent", 0);
            //shader->SetFloat3("random_color", 2.0f*3.14f * random_vector3());
            //shader->SetFloat("random_intensity", std::max(3 * random01(), 0.3f));
            //shader->SetFloat("random_F", std::max(2 * random01(), 0.3f));
            // 设置光源
            m_pipeline_settings.lights_gpu->Bind(1);
            visible_lights_id->Bind(3);
            batchData.ssbo->Bind(2);
            // 绑定VAO
            key.vao->Bind();

            // 执行实例化绘制
            uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
            RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
        }
        RenderCommand::SetDepthStencilState(DepthStencilDesc::Default());
    }

    void TransparentForwardPass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
    {
        // 使用ECS系统渲染有RenderComponent的实体
        if (!m_pipeline_settings.Scene || !m_pipeline_settings.VAOManager || !m_pipeline_settings.MatManager) {
            ENGINE_CORE_ERROR("forwardpass:Scene/VAOmanager/MatManager为空");
            return;
        }
        FBO->Bind();
        //仅清除颜色缓冲
        RenderCommand::Clear(ClearDesc::OnlyColor());
        // 根据设置选择渲染方式
        if (m_useInstancing) {
            DrawEntitiesInstanced(batch_data);
        }
        else {
            DrawEntitiesIndividually();
        }
    }

    void TransparentForwardPass::DrawEntitiesIndividually()
    {
    }

    void TransparentForwardPass::DrawEntitiesInstanced(std::unordered_map<BatchKey, BatchData, BatchKeyHash>*batch_data)
    {
        // 获取相机矩阵
        MainCamera* camera = MainCamera::GetInstance();
        if (!camera) {
            ENGINE_CORE_ERROR("No camera available for instanced rendering");
            return;
        }

        glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projMatrix = camera->GetProjectionMatrix();

        glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

        //设置混合参数
        BlendDesc blend_desc;
        blend_desc.independentBlendEnable = true;
        blend_desc.renderTarget[0] = {
                true,  // blendEnable
                BlendFactor::One, BlendFactor::One, BlendOp::Add,
                BlendFactor::Zero, BlendFactor::SrcColor, BlendOp::Add
        };
        blend_desc.renderTarget[1] = {
                false,  // blendEnable
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add,
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add
        };
        blend_desc.renderTarget[2] = {
                true,  // blendEnable
                BlendFactor::One, BlendFactor::One, BlendOp::Add,
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add
        };
        RenderCommand::SetBlendState(blend_desc);

        //仅深度测试，不深度写入
        RenderCommand::SetDepthStencilState(DepthStencilDesc::ReadOnly());
        //半透明物体绘制
        for (auto& [key, batchData] : batch_data[(int)RenderItemLayer::Transparent]) {
            if (batchData.instances.empty()) continue;

            // 绑定材质和着色器
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
            shader->SetFloat2("screen_size", glm::vec2(FBO->GetRenderTexture(0)->GetWidth(), FBO->GetRenderTexture(0)->GetHeight()));
            shader->SetInt("transparent", 1);

            // 设置光源
            m_pipeline_settings.lights_gpu->Bind(1);
            visible_lights_id->Bind(3);
            batchData.ssbo->Bind(2);
            // 绑定VAO
            key.vao->Bind();

            // 执行实例化绘制
            uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
            RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
        }
        //重新设置为不透明绘制
        auto opaque = BlendDesc::CreateOpaque();
        RenderCommand::SetBlendState(opaque);
        RenderCommand::SetDepthStencilState(DepthStencilDesc::Default());
    }

};

