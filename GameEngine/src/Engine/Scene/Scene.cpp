#include "pch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ProjectManager.h"

namespace Engine {

    void Scene::AddModelInstance(const std::string& relativeModelPath, const glm::mat4& transform) {
        SceneObject obj;
        obj.modelPath = ProjectManager::NormalizePath(relativeModelPath);
        obj.transform = transform;
        m_Objects.emplace_back(std::move(obj));
    }

    void Scene::Clear() {
        m_Objects.clear();
    }
}