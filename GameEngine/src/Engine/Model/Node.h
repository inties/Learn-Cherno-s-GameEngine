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

private:

    friend class ModelLoader;

    std::string m_Name;

    glm::mat4 m_LocalTransform;

    std::vector<Ref<Mesh>> m_Meshes;

    std::vector<Ref<Node>> m_Children;

};

} 