#include "pch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Resources/ModelManager.h"

#include "Entity.h"
#include "Component.h"
#include "Prefabs.h"
#include "ScriptableEntity.h"
#include "Engine/Utils/random.h"

namespace Engine {

    Scene::Scene() {
        // 默认创建一个CubeEntity
        for (int i = 0; i < 1000; i++) {
            Entity cube=CreatePrefab(PrefabTypes::Cube);
            glm::vec3 position = 100.0f*random_vector3()-50.0f;
            cube.GetComponent<TransformComponent>().Translate(position);
        }
        for (int i = 0; i < 1000; i++) {
            auto sphere= CreatePrefab(PrefabTypes::Sphere);
            glm::vec3 position = 100.0f * random_vector3() - 50.0f;
            sphere.GetComponent<TransformComponent>().Translate(position);
        }
        auto plane= CreatePrefab(PrefabTypes::Plane);
        plane.GetComponent<TransformComponent>().Scale(glm::vec3(1000.0f,1.0f,1000.0f));
        plane.GetComponent<TransformComponent>().Translate(glm::vec3(0, -20.0f, 0));




        // auto& nsc=cube.AddComponent<NativeScriptableComponent>();
        // nsc.Bind<MoveScript>(cube);  // 传递Entity指针而不是NativeScriptableComponent指针
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
            case PrefabTypes::Sphere:
                return SphereEntity(handle, this);
            case PrefabTypes::Plane:
                return PlaneEntity(handle, this);
            case PrefabTypes::None:
            default:
                return Entity(handle, this);
        }
    }

    Entity Scene::CreateEntity(const std::string& str)
    {
        Entity e (m_Registry.create(),this );
        if (e.HasComponent<TagComponent>()) {
            e.GetComponent<TagComponent>().Name = str;
        }
        return e;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (entity.IsValid() && (entt::entity)entity == m_SelectedEntityHandle) {
            ClearSelection();
        }
        m_Registry.destroy(entity);
        ENGINE_CORE_INFO("Entity destroyed");
    }

    void Scene::SetSelectedEntity(Entity entity)
    {
        if (entity.IsValid()) {
            m_SelectedEntityHandle = (entt::entity)entity;
            ENGINE_CORE_INFO("Selected Entity set to ID: {}", (uint32_t)entity);
        } else {
            ENGINE_CORE_WARN("Attempted to select invalid entity");
        }
    }

    Entity Scene::GetSelectedEntity() const
    {
        if (m_SelectedEntityHandle != entt::null) {
            return Entity::CreateFromHandle(m_SelectedEntityHandle, const_cast<Scene*>(this));
        }
        return Entity{}; // Return invalid entity
    }

    bool Scene::HasSelectedEntity() const
    {
        return m_SelectedEntityHandle != entt::null;
    }

    void Scene::ClearSelection()
    {
        m_SelectedEntityHandle = entt::null;
        ENGINE_CORE_INFO("Cleared entity selection");
    }

    std::vector<Entity> Scene::GetAllEntities() const
    {
        std::vector<Entity> entities;
        // Iterate through all entities in the registry
        auto view = m_Registry.view<TagComponent>();
        for(auto& entity:view) {
            entities.push_back(Entity::CreateFromHandle(entity, const_cast<Scene*>(this)));
        };
        return entities;
    }

    void Scene::CreateGameObject(const std::string& relativeModelPath, const glm::mat4& transform) {
        // 首先检查文件类型（根据后缀），如果是模型类型的文件，例如.obj等，则创建游戏对象。否则打印调试信息
        if (!IsValidModelFile(relativeModelPath)) {
            ENGINE_CORE_WARN("File {} is not a loadable model file", relativeModelPath);
            return;
        }
        ENGINE_CORE_INFO("Abandoned Function:Load Model From: {}", relativeModelPath);
        return;
        //// 创建游戏对象
        //GameObject obj;
        //obj.modelPath = ProjectManager::NormalizePath(relativeModelPath);
        //obj.transform = transform;
        //obj.isLoading = true; // 标记为正在加载状态
        //
        //// 先添加到对象列表中（显示加载状态）
        //auto objIndex = gObjectList.size();
        //gObjectList.emplace_back(std::move(obj));
        //
        //ENGINE_CORE_INFO("Game object created, starting async model loading: {}", relativeModelPath);
        //
        //// 创建异步加载任务
        //CreateAsyncModelLoadingTask(relativeModelPath, objIndex);
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
            m_SelectedObjectIndex = index;
            ENGINE_CORE_INFO("Selected ObjectID set as{}", index);       
    }
    
    GameObject* Scene::GetSelectedObject() {
        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < static_cast<int>(gObjectList.size())) {
            return &gObjectList[m_SelectedObjectIndex];
        }
        return nullptr;
    }
    
    void Scene::ClearObjectSelection() {
        m_SelectedObjectIndex = -1;
        ENGINE_CORE_INFO("Cleared GameObject selection");
    }
}
