#include "pch.h"

#include "Node.h"

#include "Engine/Renderer/Renderer.h"

namespace Engine {

void Node::Draw(const glm::mat4& parentTransform) const {
    // ���㵱ǰ�ڵ������任
    glm::mat4 worldTransform = parentTransform * m_LocalTransform;

    // ���Ƶ�ǰ�ڵ������Mesh
    for (const auto& mesh : m_Meshes) {
        mesh->SetWorldTranform(worldTransform);
        mesh->Draw();  // Mesh�ڲ������Renderer::Submit
    }

    // �ݹ�����ӽڵ�
    for (const auto& child : m_Children) {
        child->Draw(worldTransform);
    }
}

void Node::SetObjectID(int objectID) const {
    // Ϊ��ǰ�ڵ������Mesh��������ID
    for (const auto& mesh : m_Meshes) {
        mesh->SetObjectID(objectID);
        //auto material = mesh->GetMaterial();
        //if (material) {
        //    material->SetInt("u_ObjectID", objectID);
        //}
    }

    // �ݹ�Ϊ�ӽڵ���������ID
    for (const auto& child : m_Children) {
        child->SetObjectID(objectID);
    }
}

}