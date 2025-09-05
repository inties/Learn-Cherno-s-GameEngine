# GameEngine ECS架构实现指南

## 1. 什么是ECS架构

ECS（Entity-Component-System）是一种游戏开发架构模式，它将游戏对象分解为三个核心部分：

- **Entity（实体）**：轻量级的ID，不包含任何数据或逻辑
- **Component（组件）**：纯数据结构，只包含数据，不包含行为
- **System（系统）**：包含逻辑和行为，处理具有特定组件组合的实体

## 2. 为什么使用ECS

### 2.1 传统面向对象架构的问题
- 继承层次过深，难以维护
- 游戏对象变得臃肿，包含不相关的功能
- 性能问题，缓存命中率低
- 灵活性差，难以组合新功能

### 2.2 ECS的优势
- 数据导向设计，提高缓存命中率
- 组合优于继承，灵活性高
- 并行处理能力强
- 易于扩展和维护

## 3. ECS架构设计

### 3.1 核心接口设计

```cpp
// Entity只是一个轻量级的ID
typedef uint32_t Entity;

// 组件基类（纯数据结构）
struct Component {
    virtual ~Component() = default;
};

// 系统基类
class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() {}
    virtual void OnEntityAdded(Entity entity) {}
    virtual void OnEntityRemoved(Entity entity) {}
};
```

### 3.2 组件管理器设计

```cpp
class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        const char* typeName = typeid(T).name();
        m_ComponentTypes[typeName] = m_NextComponentType++;
        m_ComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    template<typename T>
    ComponentType GetComponentType() {
        const char* typeName = typeid(T).name();
        return m_ComponentTypes[typeName];
    }

private:
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char* typeName = typeid(T).name();
        return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
    }

    std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{};
    ComponentType m_NextComponentType = 0;
};
```

### 3.3 实体管理器设计

```cpp
class EntityManager {
public:
    Entity CreateEntity() {
        Entity id;
        if (m_AvailableEntities.size() > 0) {
            id = m_AvailableEntities.front();
            m_AvailableEntities.pop();
        } else {
            id = m_LivingEntityCount++;
        }
        m_Signatures[id].set();
        return id;
    }

    void DestroyEntity(Entity entity) {
        m_Signatures[entity].reset();
        m_AvailableEntities.push(entity);
    }

    void SetSignature(Entity entity, Signature signature) {
        m_Signatures[entity] = signature;
    }

    Signature GetSignature(Entity entity) {
        return m_Signatures[entity];
    }

private:
    std::queue<Entity> m_AvailableEntities{};
    std::array<Signature, MAX_ENTITIES> m_Signatures{};
    uint32_t m_LivingEntityCount = 0;
};
```

### 3.4 系统管理器设计

```cpp
class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        const char* typeName = typeid(T).name();
        auto system = std::make_shared<T>();
        m_Systems[typeName] = system;
        return system;
    }

    template<typename T>
    void SetSignature(Signature signature) {
        const char* typeName = typeid(T).name();
        m_Signatures[typeName] = signature;
    }

    void EntityDestroyed(Entity entity) {
        for (auto const& pair : m_Systems) {
            auto const& system = pair.second;
            system->OnEntityRemoved(entity);
        }
    }

    void Update(float deltaTime) {
        for (auto const& pair : m_Systems) {
            auto const& system = pair.second;
            system->Update(deltaTime);
        }
    }

private:
    std::unordered_map<const char*, Signature> m_Signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
};
```

## 4. 核心组件设计

### 4.1 变换组件

```cpp
struct TransformComponent : public Component {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 GetTransformMatrix() const {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1, 0, 0))
                           * glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0, 1, 0))
                           * glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0, 0, 1));
        
        return glm::translate(glm::mat4(1.0f), Position)
             * rotation
             * glm::scale(glm::mat4(1.0f), Scale);
    }
};
```

### 4.2 渲染组件

```cpp
struct RenderComponent : public Component {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    bool visible = true;
};
```

### 4.3 相机组件

```cpp
struct CameraComponent : public Component {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
};
```

## 5. 核心系统设计

### 5.1 渲染系统

```cpp
class RenderSystem : public System {
public:
    void Update(float deltaTime) override {
        // 收集所有需要渲染的实体
        for (auto entity : m_Entities) {
            auto& transform = m_ComponentManager->GetComponent<TransformComponent>(entity);
            auto& render = m_ComponentManager->GetComponent<RenderComponent>(entity);
            
            if (render.visible && render.mesh && render.material) {
                m_RenderQueue.push_back({entity, transform.GetTransformMatrix(), render.mesh, render.material});
            }
        }
    }

    void Render() override {
        // 按材质排序渲染队列
        std::sort(m_RenderQueue.begin(), m_RenderQueue.end(), 
            [](const RenderData& a, const RenderData& b) {
                return a.material->GetShader() < b.material->GetShader();
            });

        // 执行渲染
        for (const auto& data : m_RenderQueue) {
            data.material->Bind();
            data.material->GetShader()->SetMat4("u_Model", data.transform);
            data.mesh->Render();
        }
        
        m_RenderQueue.clear();
    }

private:
    struct RenderData {
        Entity entity;
        glm::mat4 transform;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
    };

    std::vector<RenderData> m_RenderQueue;
};
```

### 5.2 相机系统

```cpp
class CameraSystem : public System {
public:
    void Update(float deltaTime) override {
        // 更新主相机
        if (m_MainCamera != INVALID_ENTITY) {
            auto& transform = m_ComponentManager->GetComponent<TransformComponent>(m_MainCamera);
            auto& camera = m_ComponentManager->GetComponent<CameraComponent>(m_MainCamera);
            
            camera.viewMatrix = glm::lookAt(
                transform.Position,
                transform.Position + GetForwardVector(transform.Rotation),
                glm::vec3(0, 1, 0)
            );
            
            camera.projectionMatrix = glm::perspective(
                glm::radians(camera.fov),
                camera.aspectRatio,
                camera.nearPlane,
                camera.farPlane
            );
        }
    }

    void SetMainCamera(Entity entity) {
        m_MainCamera = entity;
    }

    Entity GetMainCamera() const {
        return m_MainCamera;
    }

private:
    glm::vec3 GetForwardVector(const glm::vec3& rotation) {
        return glm::normalize(glm::vec3(
            cos(rotation.y) * cos(rotation.x),
            sin(rotation.x),
            sin(rotation.y) * cos(rotation.x)
        ));
    }

    Entity m_MainCamera = INVALID_ENTITY;
};
```

### 5.3 输入系统

```cpp
class InputSystem : public System {
public:
    void Update(float deltaTime) override {
        for (auto entity : m_Entities) {
            auto& transform = m_ComponentManager->GetComponent<TransformComponent>(entity);
            
            // 处理键盘输入
            if (Input::IsKeyPressed(GLFW_KEY_W)) {
                transform.Position.z -= m_MoveSpeed * deltaTime;
            }
            if (Input::IsKeyPressed(GLFW_KEY_S)) {
                transform.Position.z += m_MoveSpeed * deltaTime;
            }
            if (Input::IsKeyPressed(GLFW_KEY_A)) {
                transform.Position.x -= m_MoveSpeed * deltaTime;
            }
            if (Input::IsKeyPressed(GLFW_KEY_D)) {
                transform.Position.x += m_MoveSpeed * deltaTime;
            }
        }
    }

private:
    float m_MoveSpeed = 5.0f;
};
```

## 6. ECS管理器整合

```cpp
class ECSManager {
public:\    void Init() {
        m_ComponentManager = std::make_unique<ComponentManager>();
        m_EntityManager = std::make_unique<EntityManager>();
        m_SystemManager = std::make_unique<SystemManager>();
    }

    Entity CreateEntity() {
        return m_EntityManager->CreateEntity();
    }

    void DestroyEntity(Entity entity) {
        m_EntityManager->DestroyEntity(entity);
        m_ComponentManager->EntityDestroyed(entity);
        m_SystemManager->EntityDestroyed(entity);
    }

    template<typename T>
    void RegisterComponent() {
        m_ComponentManager->RegisterComponent<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        m_ComponentManager->AddComponent<T>(entity, component);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        m_ComponentManager->RemoveComponent<T>(entity);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return m_SystemManager->RegisterSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature) {
        m_SystemManager->SetSignature<T>(signature);
    }

    void Update(float deltaTime) {
        m_SystemManager->Update(deltaTime);
    }

private:
    std::unique_ptr<ComponentManager> m_ComponentManager;
    std::unique_ptr<EntityManager> m_EntityManager;
    std::unique_ptr<SystemManager> m_SystemManager;
};
```

## 7. 使用示例

### 7.1 初始化ECS

```cpp
ECSManager ecs;
ecs.Init();

// 注册组件
ecs.RegisterComponent<TransformComponent>();
ecs.RegisterComponent<RenderComponent>();
ecs.RegisterComponent<CameraComponent>();

// 注册系统
auto renderSystem = ecs.RegisterSystem<RenderSystem>();
auto cameraSystem = ecs.RegisterSystem<CameraSystem>();
auto inputSystem = ecs.RegisterSystem<InputSystem>();

// 设置系统签名
Signature renderSignature;
renderSignature.set(ecs.GetComponentType<TransformComponent>());
renderSignature.set(ecs.GetComponentType<RenderComponent>());
ecs.SetSystemSignature<RenderSystem>(renderSignature);

Signature cameraSignature;
cameraSignature.set(ecs.GetComponentType<TransformComponent>());
cameraSignature.set(ecs.GetComponentType<CameraComponent>());
ecs.SetSystemSignature<CameraSystem>(cameraSignature);

Signature inputSignature;
inputSignature.set(ecs.GetComponentType<TransformComponent>());
ecs.SetSystemSignature<InputSystem>(inputSignature);
```

### 7.2 创建游戏对象

```cpp
// 创建一个立方体
Entity cube = ecs.CreateEntity();
ecs.AddComponent(cube, TransformComponent{glm::vec3(0.0f, 0.0f, -5.0f)});
ecs.AddComponent(cube, RenderComponent{
    Mesh::CreateCube(),
    Material::CreateDefault()
});

// 创建主相机
Entity camera = ecs.CreateEntity();
ecs.AddComponent(camera, TransformComponent{glm::vec3(0.0f, 0.0f, 0.0f)});
ecs.AddComponent(camera, CameraComponent{});
cameraSystem->SetMainCamera(camera);
```

### 7.3 游戏循环

```cpp
while (!glfwWindowShouldClose(window)) {
    float deltaTime = GetDeltaTime();
    
    // 更新所有系统
    ecs.Update(deltaTime);
    
    // 渲染
    renderSystem->Render();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

## 8. 性能优化建议

### 8.1 内存布局优化
- 使用SoA（Structure of Arrays）而非AoS（Array of Structures）
- 实现组件池，避免频繁的内存分配
- 使用内存对齐，提高缓存效率

### 8.2 并行处理
- 系统更新可以并行化
- 使用任务系统处理独立系统
- 实现组件数据的SIMD操作

### 8.3 缓存优化
- 按组件类型分组实体
- 减少虚函数调用
- 使用位掩码快速过滤实体

## 9. 扩展功能

### 9.1 事件系统
```cpp
class EventManager {
public:
    template<typename T>
    void Subscribe(std::function<void(const T&)> callback) {
        // 实现事件订阅
    }

    template<typename T>
    void Emit(const T& event) {
        // 触发事件
    }
};
```

### 9.2 序列化支持
- 实现组件的序列化/反序列化
- 支持场景保存和加载
- 实现预制体系统

### 9.3 脚本集成
- 集成Lua或Python脚本
- 允许脚本创建和操作实体
- 实现热重载功能

## 10. 调试工具

### 10.1 实体查看器
- 显示所有活跃实体
- 查看实体组件数据
- 实时编辑组件属性

### 10.2 性能分析器
- 系统执行时间统计
- 内存使用情况监控
- 实体数量统计

## 11. 最佳实践

1. **保持组件简单**：每个组件应该只负责一个单一职责
2. **避免系统间通信**：通过组件数据传递信息
3. **使用事件系统**：处理实体间的交互
4. **合理划分系统**：按功能而非按实体类型划分
5. **性能测试**：定期使用性能分析工具
6. **文档化**：为每个组件和系统编写清晰的文档

## 12. 总结

ECS架构为游戏引擎提供了强大的灵活性和性能优势。通过合理的设计和实现，可以构建出高性能、易扩展的游戏引擎。关键在于保持组件的纯粹性和系统的专注性，避免过度设计。

建议逐步迁移现有代码到ECS架构，先从简单的渲染系统开始，然后逐步添加更多系统和组件。