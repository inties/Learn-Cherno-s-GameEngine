#pragma once
#include "pch.h"
#include "Engine/Model/Model.h"
#include "PrefabTypes.h"
#include "Light.h"
#include "Engine/Utils/random.h"

namespace Engine {

    struct GameObject {
        std::string modelPath;
        glm::mat4 transform{ 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false;
        bool loadFailed = false;
    };
    
    class Entity;//Scene只需要创建、删除Entity，而Entity需要通过Scene的m_Registry来添加组件。因此在Scene中前向声明Entity
    class Scene {
    public:
        Scene(); // 构造函数，默认创建一个CubeEntity
        Entity CreatePrefab(PrefabTypes prefabType);
        Entity CreateEntity(const std::string& str="Undefined");//Entity本身只存储id和场景指针，因此直接返回已经创建的Entity对象的拷贝
        void DestroyEntity(Entity entity);
        
        // Entity selection management
        void SetSelectedEntity(Entity entity);
        Entity GetSelectedEntity() const;
        bool HasSelectedEntity() const;
        void ClearSelection();
        std::vector<Entity> GetAllEntities() const;
        bool IsValid() const { return true; } // Helper method for Entity validation
        
        // Legacy GameObject selection (deprecated)
        int GetSelectedID() { return m_SelectedObjectIndex; }
		entt::registry& GetRegistry() { return m_Registry; }

        //暂时弃用所有游戏对象创建、获取、选中、清理的逻辑
        //通过模型创建游戏对象，暂时弃用
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform=glm::mat4(1.0f));
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
        void ClearObjectSelection(); // Renamed to avoid conflict with Entity ClearSelection
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);
        static bool IsValidModelFile(const std::string& filePath);
        std::vector<GameObject> gObjectList;
        Ref<GameObject>currObj = nullptr;
		// ECS系统访问



        
        
    private:

     



    private:
        // Entity selection state - using entt::entity handle to avoid circular dependency
        entt::entity m_SelectedEntityHandle = entt::null;
        
        // Legacy GameObject selection (deprecated)
        int m_SelectedObjectIndex = -1;
        entt::registry m_Registry;
        friend class Entity;

        
  
    };
}
