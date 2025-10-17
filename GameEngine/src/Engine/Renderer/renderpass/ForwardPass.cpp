#include "pch.h"
#include "ForwardPass.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/camera.h"
namespace Engine {
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
                Camera* camera = Camera::GetInstance();
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

