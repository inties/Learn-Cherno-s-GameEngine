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
    
    class Entity;//Sceneֻ��Ҫ������ɾ��Entity����Entity��Ҫͨ��Scene��m_Registry���������������Scene��ǰ������Entity
    class Scene {
    public:
        Scene(); // ���캯����Ĭ�ϴ���һ��CubeEntity
        Entity CreatePrefab(PrefabTypes prefabType);
        /**
        * @brief ����entity������transform��tag���
        *
        * @param name ʵ������ƣ���ѡ�����Ϊ����Ĭ��Ϊ"Entity"��
        * @return Entity �´�����ʵ�����
        */
        Entity CreateEntity(const std::string& str="Undefined");//Entity����ֻ�洢id�ͳ���ָ�룬���ֱ�ӷ����Ѿ�������Entity����Ŀ���
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform=glm::mat4(1.0f));
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }


        std::vector<GameObject> GetGameObjects() const { return gObjectList; }


        
        
    
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
		void ClearSelection();
		
		// ECSϵͳ����
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