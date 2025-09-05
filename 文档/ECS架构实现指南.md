# GameEngine ECS�ܹ�ʵ��ָ��

## 1. ʲô��ECS�ܹ�

ECS��Entity-Component-System����һ����Ϸ�����ܹ�ģʽ��������Ϸ����ֽ�Ϊ�������Ĳ��֣�

- **Entity��ʵ�壩**����������ID���������κ����ݻ��߼�
- **Component�������**�������ݽṹ��ֻ�������ݣ���������Ϊ
- **System��ϵͳ��**�������߼�����Ϊ����������ض������ϵ�ʵ��

## 2. Ϊʲôʹ��ECS

### 2.1 ��ͳ�������ܹ�������
- �̳в�ι������ά��
- ��Ϸ������ӷ�ף���������صĹ���
- �������⣬���������ʵ�
- ����Բ��������¹���

### 2.2 ECS������
- ���ݵ�����ƣ���߻���������
- ������ڼ̳У�����Ը�
- ���д�������ǿ
- ������չ��ά��

## 3. ECS�ܹ����

### 3.1 ���Ľӿ����

```cpp
// Entityֻ��һ����������ID
typedef uint32_t Entity;

// ������ࣨ�����ݽṹ��
struct Component {
    virtual ~Component() = default;
};

// ϵͳ����
class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() {}
    virtual void OnEntityAdded(Entity entity) {}
    virtual void OnEntityRemoved(Entity entity) {}
};
```

### 3.2 ������������

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

### 3.3 ʵ����������

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

### 3.4 ϵͳ���������

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

## 4. ����������

### 4.1 �任���

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

### 4.2 ��Ⱦ���

```cpp
struct RenderComponent : public Component {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    bool visible = true;
};
```

### 4.3 ������

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

## 5. ����ϵͳ���

### 5.1 ��Ⱦϵͳ

```cpp
class RenderSystem : public System {
public:
    void Update(float deltaTime) override {
        // �ռ�������Ҫ��Ⱦ��ʵ��
        for (auto entity : m_Entities) {
            auto& transform = m_ComponentManager->GetComponent<TransformComponent>(entity);
            auto& render = m_ComponentManager->GetComponent<RenderComponent>(entity);
            
            if (render.visible && render.mesh && render.material) {
                m_RenderQueue.push_back({entity, transform.GetTransformMatrix(), render.mesh, render.material});
            }
        }
    }

    void Render() override {
        // ������������Ⱦ����
        std::sort(m_RenderQueue.begin(), m_RenderQueue.end(), 
            [](const RenderData& a, const RenderData& b) {
                return a.material->GetShader() < b.material->GetShader();
            });

        // ִ����Ⱦ
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

### 5.2 ���ϵͳ

```cpp
class CameraSystem : public System {
public:
    void Update(float deltaTime) override {
        // ���������
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

### 5.3 ����ϵͳ

```cpp
class InputSystem : public System {
public:
    void Update(float deltaTime) override {
        for (auto entity : m_Entities) {
            auto& transform = m_ComponentManager->GetComponent<TransformComponent>(entity);
            
            // �����������
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

## 6. ECS����������

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

## 7. ʹ��ʾ��

### 7.1 ��ʼ��ECS

```cpp
ECSManager ecs;
ecs.Init();

// ע�����
ecs.RegisterComponent<TransformComponent>();
ecs.RegisterComponent<RenderComponent>();
ecs.RegisterComponent<CameraComponent>();

// ע��ϵͳ
auto renderSystem = ecs.RegisterSystem<RenderSystem>();
auto cameraSystem = ecs.RegisterSystem<CameraSystem>();
auto inputSystem = ecs.RegisterSystem<InputSystem>();

// ����ϵͳǩ��
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

### 7.2 ������Ϸ����

```cpp
// ����һ��������
Entity cube = ecs.CreateEntity();
ecs.AddComponent(cube, TransformComponent{glm::vec3(0.0f, 0.0f, -5.0f)});
ecs.AddComponent(cube, RenderComponent{
    Mesh::CreateCube(),
    Material::CreateDefault()
});

// ���������
Entity camera = ecs.CreateEntity();
ecs.AddComponent(camera, TransformComponent{glm::vec3(0.0f, 0.0f, 0.0f)});
ecs.AddComponent(camera, CameraComponent{});
cameraSystem->SetMainCamera(camera);
```

### 7.3 ��Ϸѭ��

```cpp
while (!glfwWindowShouldClose(window)) {
    float deltaTime = GetDeltaTime();
    
    // ��������ϵͳ
    ecs.Update(deltaTime);
    
    // ��Ⱦ
    renderSystem->Render();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

## 8. �����Ż�����

### 8.1 �ڴ沼���Ż�
- ʹ��SoA��Structure of Arrays������AoS��Array of Structures��
- ʵ������أ�����Ƶ�����ڴ����
- ʹ���ڴ���룬��߻���Ч��

### 8.2 ���д���
- ϵͳ���¿��Բ��л�
- ʹ������ϵͳ�������ϵͳ
- ʵ��������ݵ�SIMD����

### 8.3 �����Ż�
- ��������ͷ���ʵ��
- �����麯������
- ʹ��λ������ٹ���ʵ��

## 9. ��չ����

### 9.1 �¼�ϵͳ
```cpp
class EventManager {
public:
    template<typename T>
    void Subscribe(std::function<void(const T&)> callback) {
        // ʵ���¼�����
    }

    template<typename T>
    void Emit(const T& event) {
        // �����¼�
    }
};
```

### 9.2 ���л�֧��
- ʵ����������л�/�����л�
- ֧�ֳ�������ͼ���
- ʵ��Ԥ����ϵͳ

### 9.3 �ű�����
- ����Lua��Python�ű�
- ����ű������Ͳ���ʵ��
- ʵ�������ع���

## 10. ���Թ���

### 10.1 ʵ��鿴��
- ��ʾ���л�Ծʵ��
- �鿴ʵ���������
- ʵʱ�༭�������

### 10.2 ���ܷ�����
- ϵͳִ��ʱ��ͳ��
- �ڴ�ʹ��������
- ʵ������ͳ��

## 11. ���ʵ��

1. **���������**��ÿ�����Ӧ��ֻ����һ����һְ��
2. **����ϵͳ��ͨ��**��ͨ��������ݴ�����Ϣ
3. **ʹ���¼�ϵͳ**������ʵ���Ľ���
4. **������ϵͳ**�������ܶ��ǰ�ʵ�����ͻ���
5. **���ܲ���**������ʹ�����ܷ�������
6. **�ĵ���**��Ϊÿ�������ϵͳ��д�������ĵ�

## 12. �ܽ�

ECS�ܹ�Ϊ��Ϸ�����ṩ��ǿ�������Ժ��������ơ�ͨ���������ƺ�ʵ�֣����Թ����������ܡ�����չ����Ϸ���档�ؼ����ڱ�������Ĵ����Ժ�ϵͳ��רע�ԣ����������ơ�

������Ǩ�����д��뵽ECS�ܹ����ȴӼ򵥵���Ⱦϵͳ��ʼ��Ȼ������Ӹ���ϵͳ�������