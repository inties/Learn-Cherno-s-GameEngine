#pragma once
#include "pch.h"
#include "Engine/Model/Model.h"
#include "PrefabTypes.h"


namespace Engine {

    struct GameObject {
        std::string modelPath;
        glm::mat4 transform{ 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false;
        bool loadFailed = false;
    };
    
    class Entity;//Sceneֻ��Ҫ������ɾ��Entity����Entity��Ҫͨ��Scene��m_Registry���������������Scene��ǰ������Entity
    class Scene {
    public:
        Scene(); // ���캯����Ĭ�ϴ���һ��CubeEntity
        Entity CreatePrefab(PrefabTypes prefabType);
        Entity CreateEntity(const std::string& str="Undefined");//Entity����ֻ�洢id�ͳ���ָ�룬���ֱ�ӷ����Ѿ�������Entity����Ŀ���
        int GetSelectedID() { return m_SelectedObjectIndex; }
		entt::registry& GetRegistry() { return m_Registry; }



        //��ʱ����������Ϸ���󴴽�����ȡ��ѡ�С�������߼�
        //ͨ��ģ�ʹ�����Ϸ������ʱ����
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform=glm::mat4(1.0f));
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }
        std::vector<GameObject> GetGameObjects() const { return gObjectList; }
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
		void ClearSelection();
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);
        static bool IsValidModelFile(const std::string& filePath);
        std::vector<GameObject> gObjectList;
        Ref<GameObject>currObj = nullptr;

		
		// ECSϵͳ����

    private:
      
        int m_SelectedObjectIndex = -1; 
        entt::registry m_Registry;
        friend class Entity;
    };
}