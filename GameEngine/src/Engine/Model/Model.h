#pragma once

#include "pch.h"
#include "Node.h"
#include "Engine/Renderer/VertexArray.h"

namespace Engine {

class Model {
public:
    static Ref<Model> Create(const std::string& path);

    void Draw() const;
    
    // 设置全局变换
    void SetGlobalTransform(const glm::mat4& transform) { m_GlobalTransform = transform; }
    const glm::mat4& GetGlobalTransform() const { return m_GlobalTransform; }
    
    // 为模型的所有mesh设置物体ID
    void SetObjectID(int objectID) const;
    
    // 调试接口：获取共享VAO
    const Ref<VertexArray>& GetSharedVAO() const { return m_SharedVAO; }
    
    Model() = default;

private:
    friend class ModelLoader;

    Ref<Node> m_RootNode;
    glm::mat4 m_GlobalTransform = glm::mat4(1.0f);  // 模型整体变换
    Ref<VertexArray> m_SharedVAO;  // 共享VAO引用（用于传递给子Mesh）
};

}