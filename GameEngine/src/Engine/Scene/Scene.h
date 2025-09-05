#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Engine/core.h"
#include "Engine/Model/Model.h"

namespace Engine {

    struct SceneObject {
        std::string modelPath; // 相对项目根的模型路径
        glm::mat4 transform { 1.0f };
    };

    class Scene {
    public:
        void AddModelInstance(const std::string& relativeModelPath, const glm::mat4& transform);
        const std::vector<SceneObject>& GetObjects() const { return m_Objects; }

        // 简化：获取所有模型实例（后续可按类型分组）
        std::vector<SceneObject> GetModelObjects() const { return m_Objects; }

        void Clear();
    private:
        std::vector<SceneObject> m_Objects;
    };
}