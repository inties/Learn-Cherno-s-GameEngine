#include "pch.h"

#include "Node.h"

#include "Engine/Renderer/Renderer.h"

namespace Engine {

void Node::Draw(const glm::mat4& parentTransform) const {
    // ���㵱ǰ�ڵ������任
    glm::mat4 worldTransform = parentTransform * m_LocalTransform;

    // ���Ƶ�ǰ�ڵ������Mesh
    for (const auto& mesh : m_Meshes) {
        mesh->Draw(worldTransform);  // Mesh�ڲ������Renderer::Submit
    }

    // �ݹ�����ӽڵ�
    for (const auto& child : m_Children) {
        child->Draw(worldTransform);
    }
}

} 