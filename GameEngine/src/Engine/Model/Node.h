#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Mesh.h"

#include <glm/glm.hpp>

#include <vector>

#include <string>

namespace Engine {

class Node {

public:

    void Draw(const glm::mat4& parentTransform) const;
    
    // 为节点及其子节点的所有mesh设置物体ID
    void SetObjectID(int objectID) const;
    void SetLocalTransform(glm::mat4& transform) { m_LocalTransform = transform; }

private:

    friend class ModelLoader;

    std::string m_Name;

    glm::mat4 m_LocalTransform=glm::mat4 (1.0f);

    std::vector<Ref<Mesh>> m_Meshes;

    std::vector<Ref<Node>> m_Children;

};

}