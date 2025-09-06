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
        std::string modelPath; // �����Ŀ����ģ��·��
        glm::mat4 transform { 1.0f };
        std::weak_ptr<Model> model;
        bool isLoading = false; // �Ƿ����ڼ���
        bool loadFailed = false; // �Ƿ����ʧ��
    };

    class Scene {
    public:
        void CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform);
        const std::vector<GameObject>& GetObjects() const { return gObjectList; }

        // �򻯣���ȡ����ģ��ʵ���������ɰ����ͷ��飩
        std::vector<GameObject> GetModelObjects() const { return gObjectList; }

        void Clear();
        
        // �����첽��������״̬
        void UpdateLoadingTasks();
        
        // ѡ�ж������
        void SetSelectedObject(int index);
        int GetSelectedObjectIndex() const { return m_SelectedObjectIndex; }
        GameObject* GetSelectedObject();
        const GameObject* GetSelectedObject() const;
        void ClearSelection();

    private:
        // ����ļ��Ƿ�Ϊ֧�ֵ�ģ�͸�ʽ
        static bool IsValidModelFile(const std::string& filePath);
        
        // �����첽ģ�ͼ�������
        void CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex);

    private:
        std::vector<GameObject> gObjectList;
        std::vector<std::future<void>> m_LoadingTasks; // �첽��������
        Ref<GameObject>currObj = nullptr;
        int m_SelectedObjectIndex = -1; // ��ǰѡ�еĶ���������-1��ʾδѡ��
    };
}