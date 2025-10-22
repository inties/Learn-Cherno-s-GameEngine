#include "pch.h"
#include "ForwardPass.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/camera.h"
namespace Engine {
    void ForwardPass::InitEnvMapPass() {
       /* auto envmap = CreateScope<TextureCube>(TextureFormat::RGB16,32,32);*/
    }
    void ForwardPass::DrawEnvCube()
    {
        //auto cubeVAO=Spec.VAOManager->Get("Cube").get();
        //glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        //cubeVAO->Bind();
        ////m_TextureCube->Bind(0);
        //envMap->Bind(0);
        //m_shader->Bind();
        //MainCamera* camera = MainCamera::GetInstance();
        //glm::mat4 viewMatrix = camera->GetViewMatrix();


        //glm::mat4 projMatrix = camera->GetProjectionMatrix();
        //glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
        //m_shader->SetMat4("u_ViewProjection", viewProjMatrix);
        //FBO->Bind();
        //RenderCommand::DrawIndexed(m_cubeVAO);
        //glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content
    }
    void ForwardPass::Draw() {
        // 使用ECS系统渲染有RenderComponent的实体
        if (!Spec.scene || !Spec.VAOManager || !Spec.MatManager) {
            ENGINE_CORE_ERROR("forwardpass:Scene/VAOmanager/MatManager为空");
            return;
        }

        // 根据设置选择渲染方式
        if (Spec.useInstancing) {
            DrawEntitiesInstanced();
        } else {
            DrawEntitiesIndividually();
        }
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

    void ForwardPass::DrawEntitiesInstanced() {
        // 收集实例数据
        m_InstancedRenderer->CollectInstances(Spec.scene, Spec.VAOManager, Spec.MatManager);
        
        // 执行实例化渲染
        m_InstancedRenderer->RenderInstanced();
    }

};

