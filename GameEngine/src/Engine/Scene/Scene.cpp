#include "pch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Resources/ModelManager.h"
#include <filesystem>

#include "Entity.h"
#include "Component.h"
#include "prefabs.h"
namespace Engine {

    Scene::Scene() {
        // 默认创建一个CubeEntity
        CreatePrefab(PrefabTypes::Cube);
        ENGINE_CORE_INFO("Scene created with default CubeEntity");
    }

    // 检查文件是否为支持的模型格式
    bool Scene::IsValidModelFile(const std::string& filePath) {
        std::string extension = std::filesystem::path(filePath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        static const std::set<std::string> supportedFormats = {
            ".obj", ".fbx", ".dae", ".gltf", ".glb", ".3ds", ".ply", ".x", ".md2", ".md3"
        };
        
        return supportedFormats.find(extension) != supportedFormats.end();
    }

    Entity Scene::CreatePrefab(PrefabTypes prefabType)
    {
        entt::entity handle = m_Registry.create();
        switch (prefabType) {
            case PrefabTypes::Cube:
                return CubeEntity(handle, this);
            case PrefabTypes::None:
            default:
                return Entity(handle, this);
        }
    }

    Entity Scene::CreateEntity(const std::string& str)
    {
        Entity e (m_Registry.create(),this );
        e.AddComponent<TransformComponent>(glm::vec3(1.0f, 1.0f, 1.0f));
        e.AddComponent<TagComponent>(str);
        return e;
    }

    void Scene::CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform) {
        // 首先检查文件类型（根据后缀），如果是模型类型的文件，例如.obj等，则创建游戏对象。否则打印调试信息
        if (!IsValidModelFile(relativeModelPath)) {
            ENGINE_CORE_WARN("File {} is not a loadable model file", relativeModelPath);
            return;
        }

        // 创建游戏对象
        GameObject obj;
        obj.modelPath = ProjectManager::NormalizePath(relativeModelPath);
        obj.transform = transform;
        obj.isLoading = true; // 标记为正在加载状态
        
        // 先添加到对象列表中（显示加载状态）
        auto objIndex = gObjectList.size();
        gObjectList.emplace_back(std::move(obj));
        
        ENGINE_CORE_INFO("Game object created, starting async model loading: {}", relativeModelPath);
        
        // 创建异步加载任务
        CreateAsyncModelLoadingTask(relativeModelPath, objIndex);
    }

    void Scene::CreateAsyncModelLoadingTask(const std::string& relativeModelPath, size_t objectIndex) {
        //// 使用 std::async 创建异步任务
        //auto future = std::async(std::launch::async, [this, relativeModelPath, objectIndex]() {
        //    ENGINE_CORE_INFO("Async loading task started: {}", relativeModelPath);
            
          
        auto ModelManager = ModelManager::Get();
        Ref<Model> model_ptr = nullptr;
                
        if (ModelManager->IsModelLoaded(relativeModelPath)) {
            ENGINE_CORE_INFO("Model already in cache, getting directly: {}", relativeModelPath);
            model_ptr = ModelManager->GetModel(relativeModelPath);
        } else {
            ENGINE_CORE_INFO("Starting to load model from file: {}", relativeModelPath);
            model_ptr = ModelManager->LoadModel(relativeModelPath);
        }
                
        if (model_ptr) {
            ENGINE_CORE_INFO("Model loaded successfully: {}", relativeModelPath);
                    
          
            gObjectList[objectIndex].model = model_ptr;
            gObjectList[objectIndex].isLoading = false;
            
        }
        else {
            ENGINE_CORE_ERROR("Model loading failed: {}", relativeModelPath);

            // 标记加载失败
           
            gObjectList[objectIndex].isLoading = false;
            gObjectList[objectIndex].loadFailed = true;
            

            }
            //}
       
        
        //// 存储 future 以便后续管理
        //m_LoadingTasks.emplace_back(std::move(future));
    }


    
    void Scene::SetSelectedObject(int index) {
        if (index >= 0 && index < static_cast<int>(gObjectList.size())) {
            m_SelectedObjectIndex = index;
            ENGINE_CORE_INFO("Selected object at index: {}", index);
        } else if (index == -1) {
            m_SelectedObjectIndex = -1; // 清除选择
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
    
    void Scene::ClearSelection() {
        m_SelectedObjectIndex = -1;
        ENGINE_CORE_INFO("Cleared object selection");
    }
}