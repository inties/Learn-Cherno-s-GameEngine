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
        std::string modelPath; 
        glm::mat4 transform { 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false; 
        bool loadFailed = false; 
    };

    class Scene {
    public:

        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform=glm::mat4(1.0f));
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }


        std::vector<GameObject> GetGameObjects() const { return gObjectList; }

        void Clear();
        
        
    
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
        void ClearSelection();

    private:
        static bool IsValidModelFile(const std::string& filePath);
        
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);

    private:
        std::vector<GameObject> gObjectList;
        std::vector<std::future<void>> m_LoadingTasks; // ??????????
        Ref<GameObject>currObj = nullptr;
        int m_SelectedObjectIndex = -1; // ?????§Ö??????????-1???¦Ä???
    };
}