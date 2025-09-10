#include "pch.h"

#include "Node.h"

#include "Engine/Renderer/Renderer.h"

namespace Engine {

void Node::Draw(const glm::mat4& parentTransform) const {
    // 计算当前节点的世界变换
    glm::mat4 worldTransform = parentTransform * m_LocalTransform;

    // 绘制当前节点的所有Mesh
    for (const auto& mesh : m_Meshes) {
        mesh->SetWorldTranform(worldTransform);
        mesh->Draw();  // Mesh内部会调用Renderer::Submit
    }

    // 递归绘制子节点
    for (const auto& child : m_Children) {
        child->Draw(worldTransform);
    }
}

void Node::SetObjectID(int objectID) const {
    // 为当前节点的所有Mesh设置物体ID
    for (const auto& mesh : m_Meshes) {
        mesh->SetObjectID(objectID);
        //auto material = mesh->GetMaterial();
        //if (material) {
        //    material->SetInt("u_ObjectID", objectID);
        //}
    }

    // 递归为子节点设置物体ID
    for (const auto& child : m_Children) {
        child->SetObjectID(objectID);
    }
}

}