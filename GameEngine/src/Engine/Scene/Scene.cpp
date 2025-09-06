#include "pch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Resources/ResourceManager.h"
#include "Engine/log.h"
#include <filesystem>
#include <future>
#include <thread>
#include <algorithm>
#include <set>
#include <chrono>

namespace Engine {

    // ����ļ��Ƿ�Ϊ֧�ֵ�ģ�͸�ʽ
    bool Scene::IsValidModelFile(const std::string& filePath) {
        std::string extension = std::filesystem::path(filePath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        static const std::set<std::string> supportedFormats = {
            ".obj", ".fbx", ".dae", ".gltf", ".glb", ".3ds", ".ply", ".x", ".md2", ".md3"
        };
        
        return supportedFormats.find(extension) != supportedFormats.end();
    }

    void Scene::CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform) {
        ENGINE_CORE_INFO("Starting to create game object, model path: {}", relativeModelPath);
        
        // ���ȼ���ļ����ͣ����ݺ�׺���������ģ�����͵��ļ�������.obj�ȣ��򴴽���Ϸ���󡣷����ӡ������Ϣ
        if (!IsValidModelFile(relativeModelPath)) {
            ENGINE_CORE_WARN("File {} is not a loadable model file", relativeModelPath);
            return;
        }

        // ������Ϸ����
        GameObject obj;
        obj.modelPath = ProjectManager::NormalizePath(relativeModelPath);
        obj.transform = transform;
        obj.isLoading = true; // ���Ϊ���ڼ���״̬
        
        // ����ӵ������б��У���ʾ����״̬��
        auto objIndex = gObjectList.size();
        gObjectList.emplace_back(std::move(obj));
        
        ENGINE_CORE_INFO("Game object created, starting async model loading: {}", relativeModelPath);
        
        // �����첽��������
        CreateAsyncModelLoadingTask(relativeModelPath, objIndex);
    }

    void Scene::CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex) {
        // ʹ�� std::async �����첽����
        auto future = std::async(std::launch::async, [this, relativeModelPath, objectIndex]() {
            ENGINE_CORE_INFO("Async loading task started: {}", relativeModelPath);
            
            try {
                auto resourceManager = ResourceManager::Get();
                Ref<Model> model_ptr = nullptr;
                
                if (resourceManager->IsModelLoaded(relativeModelPath)) {
                    ENGINE_CORE_INFO("Model already in cache, getting directly: {}", relativeModelPath);
                    model_ptr = resourceManager->GetModel(relativeModelPath);
                } else {
                    ENGINE_CORE_INFO("Starting to load model from file: {}", relativeModelPath);
                    model_ptr = resourceManager->LoadModel(relativeModelPath);
                }
                
                if (model_ptr) {
                    ENGINE_CORE_INFO("Model loaded successfully: {}", relativeModelPath);
                    
                    // ������Ϸ����
                    if (objectIndex < gObjectList.size()) {
                        gObjectList[objectIndex].model = model_ptr;
                        gObjectList[objectIndex].isLoading = false;
                        ENGINE_CORE_INFO("Game object model binding completed: {}", relativeModelPath);
                    }
                } else {
                    ENGINE_CORE_ERROR("Model loading failed: {}", relativeModelPath);
                    
                    // ��Ǽ���ʧ��
                    if (objectIndex < gObjectList.size()) {
                        gObjectList[objectIndex].isLoading = false;
                        gObjectList[objectIndex].loadFailed = true;
                    }
                }
            } catch (const std::exception& e) {
                ENGINE_CORE_ERROR("Model loading exception: {} - {}", relativeModelPath, e.what());
                
                if (objectIndex < gObjectList.size()) {
                    gObjectList[objectIndex].isLoading = false;
                    gObjectList[objectIndex].loadFailed = true;
                }
            }
        });
        
        // �洢 future �Ա��������
        m_LoadingTasks.emplace_back(std::move(future));
    }

    void Scene::UpdateLoadingTasks() {
        // ��������ɵ�����
        auto it = std::remove_if(m_LoadingTasks.begin(), m_LoadingTasks.end(),
            [](std::future<void>& future) {
                if (future.valid()) {
                    auto status = future.wait_for(std::chrono::seconds(0));
                    return status == std::future_status::ready;
                }
                return true; // �Ƴ���Ч��future
            });
        
        m_LoadingTasks.erase(it, m_LoadingTasks.end());
    }

    void Scene::Clear() {
        // �ȴ����м����������
        for (auto& task : m_LoadingTasks) {
            if (task.valid()) {
                task.wait();
            }
        }
        m_LoadingTasks.clear();
        gObjectList.clear();
        m_SelectedObjectIndex = -1; // ���ѡ��
    }
    
    void Scene::SetSelectedObject(int index) {
        if (index >= 0 && index < static_cast<int>(gObjectList.size())) {
            m_SelectedObjectIndex = index;
            ENGINE_CORE_INFO("Selected object at index: {}", index);
        } else if (index == -1) {
            m_SelectedObjectIndex = -1; // ���ѡ��
            ENGINE_CORE_INFO("Cleared object selection");
        } else {
            ENGINE_CORE_WARN("Invalid object index: {}", index);
        }
    }
    
    GameObject* Scene::GetSelectedObject() {
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < static_cast<int>(gObjectList.size())) {
            return &gObjectList[m_SelectedObjectIndex];
        }
        return nullptr;
    }
    
    const GameObject* Scene::GetSelectedObject() const {
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < static_cast<int>(gObjectList.size())) {
            return &gObjectList[m_SelectedObjectIndex];
        }
        return nullptr;
    }
    
    void Scene::ClearSelection() {
        m_SelectedObjectIndex = -1;
        ENGINE_CORE_INFO("Cleared object selection");
    }
}