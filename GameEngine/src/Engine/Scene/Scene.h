#pragma once
#include "pch.h"
#include "Engine/Model/Model.h"
#include "PrefabTypes.h"
//#include "entt.hpp"
#include "E:\myGitRepos\GameEngine\GameEngine\dependency\entt\src\entt\entt.hpp"
namespace Engine {

    struct GameObject {
        std::string modelPath; 
        glm::mat4 transform { 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false; 
        bool loadFailed = false; 
    };
    
    class Entity;//Scene只需要创建、删除Entity，而Entity需要通过Scene的m_Registry来添加组件。因此在Scene中前向声明Entity
    class Scene {
    public:
        Scene(); // 构造函数，默认创建一个CubeEntity
        Entity CreatePrefab(PrefabTypes prefabType);
        /**
        * @brief 创建entity并附加transform和tag组件
        *
        * @param name 实体的名称（可选，如果为空则默认为"Entity"）
        * @return Entity 新创建的实体对象
        */
        Entity CreateEntity(const std::string& str="Undefined");//Entity本身只存储id和场景指针，因此直接返回已经创建的Entity对象的拷贝
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform=glm::mat4(1.0f));
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }


        std::vector<GameObject> GetGameObjects() const { return gObjectList; }


        
        
    
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
		void ClearSelection();
		
		// ECS系统访问
		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

	private:
        static bool IsValidModelFile(const std::string& filePath);
        
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);

    private:
        std::vector<GameObject> gObjectList;
        Ref<GameObject>currObj = nullptr;
        int m_SelectedObjectIndex = -1; 
        entt::registry m_Registry;
        friend class Entity;
    };
}