#pragma once

#include <string>
#include <vector>
#include <set>
#include <future>
#include <glm/glm.hpp>
#include "Engine/core.h"
#include "Engine/Model/Model.h"

namespace Engine {

    struct GameObject {
        std::string modelPath; // 相对项目根的模型路径
        glm::mat4 transform { 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false; // 是否正在加载
        bool loadFailed = false; // 是否加载失败
    };

    class Scene {
    public:
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform);
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }

        // 简化：获取所有模型实例（后续可按类型分组）
        std::vector<GameObject> GetModelObjects() const { return gObjectList; }

        void Clear();
        
        // 更新异步加载任务状态
        void UpdateLoadingTasks();
        
        // 选中对象管理
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
        const GameObject* GetSelectedObject() const;
        void ClearSelection();

    private:
        // 检查文件是否为支持的模型格式
        static bool IsValidModelFile(const std::string& filePath);
        
        // 创建异步模型加载任务
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);

    private:
        std::vector<GameObject> gObjectList;
        std::vector<std::future<void>> m_LoadingTasks; // 异步加载任务
        Ref<GameObject>currObj = nullptr;
        int m_SelectedObjectIndex = -1; // 当前选中的对象索引，-1表示未选中
    };
}