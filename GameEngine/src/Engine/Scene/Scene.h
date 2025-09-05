#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Engine/core.h"
#include "Engine/Model/Model.h"

namespace Engine {

    struct SceneObject {
        std::string modelPath; // �����Ŀ����ģ��·��
        glm::mat4 transform { 1.0f };
    };

    class Scene {
    public:
        void AddModelInstance(const std::string& relativeModelPath, const glm::mat4& transform);
        const std::vector<SceneObject>& GetObjects() const { return m_Objects; }

        // �򻯣���ȡ����ģ��ʵ���������ɰ����ͷ��飩
        std::vector<SceneObject> GetModelObjects() const { return m_Objects; }

        void Clear();
    private:
        std::vector<SceneObject> m_Objects;
    };
}