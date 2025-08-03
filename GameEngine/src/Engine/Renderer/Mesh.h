#pragma once

#include "Engine/core.h"
#include "Engine/Renderer/Vertex.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Material.h"
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <memory>

namespace Engine {

class Mesh : public std::enable_shared_from_this<Mesh> {
public:
    // 构造函数：接收材质和偏移信息，不再创建缓冲区
    Mesh(const Ref<Material>& material, const Ref<VertexArray>& sharedVAO, 
         uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount);

    // Draw方法：使用全局camera和transform，调用Renderer::Submit
    void Draw(const glm::mat4& transform) const;

    // Getters
    Ref<VertexArray> GetSharedVAO() const { return m_SharedVAO; }
    uint32_t GetVertexOffset() const { return m_VertexOffset; }
    uint32_t GetVertexCount() const { return m_VertexCount; }
    uint32_t GetIndexOffset() const { return m_IndexOffset; }
    uint32_t GetIndexCount() const { return m_IndexCount; }
    Ref<Material> GetMaterial() const { return m_Material; }

private:
    Ref<VertexArray> m_SharedVAO;    // 共享VAO引用
    Ref<Material> m_Material;       // 材质引用
    uint32_t m_VertexOffset;        // 顶点缓冲区起始偏移
    uint32_t m_VertexCount;         // 顶点数量
    uint32_t m_IndexOffset;         // 索引缓冲区起始偏移
    uint32_t m_IndexCount;          // 索引数量
};

} 