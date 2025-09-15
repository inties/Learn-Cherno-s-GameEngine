#pragma once

#include "pch.h"
#include "Node.h"
#include "Engine/Renderer/VertexArray.h"

namespace Engine {

class Model {
public:
    static Ref<Model> Create(const std::string& path);

    void Draw() const;
    
    // ����ȫ�ֱ任
    void SetGlobalTransform(const glm::mat4& transform) { m_GlobalTransform = transform; }
    const glm::mat4& GetGlobalTransform() const { return m_GlobalTransform; }
    
    // Ϊģ�͵�����mesh��������ID
    void SetObjectID(int objectID) const;
    
    // ���Խӿڣ���ȡ����VAO
    const Ref<VertexArray>& GetSharedVAO() const { return m_SharedVAO; }
    
    Model() = default;

private:
    friend class ModelLoader;

    Ref<Node> m_RootNode;
    glm::mat4 m_GlobalTransform = glm::mat4(1.0f);  // ģ������任
    Ref<VertexArray> m_SharedVAO;  // ����VAO���ã����ڴ��ݸ���Mesh��
};

}