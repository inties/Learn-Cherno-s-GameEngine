#include "pch.h"
#include "Mesh.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/CameraManager.h"

namespace Engine {

Mesh::Mesh(const Ref<Material>& material, const Ref<VertexArray>& sharedVAO, 
           uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount)
    : m_Material(material), m_SharedVAO(sharedVAO), 
      m_VertexOffset(vertexOffset), m_VertexCount(vertexCount),
      m_IndexOffset(indexOffset), m_IndexCount(indexCount) {
}

void Mesh::Draw() const {
    // 调用Renderer::Submit，传入transform，使用全局camera
    m_Material->SetInt("u_ObjectID", static_cast<int>(m_ObjectID));
    Renderer::Submit(std::const_pointer_cast<Mesh>(shared_from_this()), worldTransform);
}

}