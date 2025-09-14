# Design Document

## Overview

This design implements a component lifecycle notification system that automatically creates and manages GPU resources when RenderComponents are added to entities. The system uses lazy resource creation to avoid blocking during entity creation, with GPU resources created just-in-time during the render pass.

The core architecture introduces component lifecycle callbacks in the Scene class, a resource factory system in RendererLayer, and shared resource management to optimize memory usage when multiple entities use the same geometry (like cubes).

## Feasibility Analysis

### Lazy Resource Creation Approach

**Advantages:**
- **Non-blocking Entity Creation**: Entities can be created instantly without GPU operations
- **Guaranteed OpenGL Context**: Resources are created during render when context is active
- **Batch Processing**: Multiple entities can have resources created efficiently in one render pass
- **Memory Efficiency**: Only entities that get rendered will have GPU resources allocated

**Implementation Feasibility:**
- ✅ **Single-threaded Compatibility**: Works perfectly in current single-threaded architecture
- ✅ **Existing Code Reuse**: Can directly reuse SetupCube() logic without modification
- ✅ **Minimal Performance Impact**: Resource validation is a simple null check per entity
- ✅ **Error Handling**: Failed creation attempts can be cached to avoid repeated failures

**Potential Concerns & Solutions:**
- **First Frame Stutter**: Multiple resources created in first render frame
  - *Solution*: Spread creation across multiple frames with a creation budget per frame
- **Resource Creation Failure**: GPU resource allocation might fail
  - *Solution*: Graceful fallback with error logging and retry mechanisms

## Architecture

### Component Lifecycle System

The system extends the existing ECS architecture with lifecycle notifications:

```cpp
// In Entity class,not in Scene class
template<typename T>
void OnComponentAdded(Entity entity, T& component);

template<typename T> 
void OnComponentRemoved(Entity entity, T& component);
```

These callbacks are triggered automatically when components are added or removed through the Entity interface, creating a clean separation between ECS operations and system-specific resource management.

### Separate Resource Factory Pattern

The RendererLayer manages geometry and material resources separately:

```cpp
// In RendererLayer class
class GeometryManager {
public:
    Ref<VertexArray> GetOrCreateGeometry(const std::string& geometryName);
    void ReleaseGeometry(const std::string& geometryName);
    bool IsGeometryLoaded(const std::string& geometryName);
private:
    std::unordered_map<std::string, std::pair<Ref<VertexArray>, int>> m_GeometryCache; // VAO + ref count
};

class MaterialManager {
public:
    Ref<Material> GetOrCreateMaterial(const std::string& materialName);
    void ReleaseMaterial(const std::string& materialName);
    bool IsMaterialLoaded(const std::string& materialName);
private:
    std::unordered_map<std::string, std::pair<Ref<Material>, int>> m_MaterialCache; // Material + ref count
};

// RendererLayer methods
Ref<VertexArray> CreateCubeGeometry();
Ref<Material> CreateDefaultCubeMaterial();
bool CreateResourcesForComponent(RenderComponent& component);
```

### Service Locator for RendererLayer Access

The Scene class needs access to RendererLayer for resource creation. This is achieved through a service locator pattern:

```cpp
class RenderResourceManager {
public:
    static void SetRendererLayer(RendererLayer* renderer);
    static RendererLayer* GetRendererLayer();
    static GPUResourceHandle CreateResourcesForComponent(const std::string& type);
};
```

## Components and Interfaces

### Enhanced RenderComponent

The RenderComponent supports separate geometry and material resource management:

```cpp
struct RenderComponent {
    // GPU Resources
    Ref<VertexArray> VAO = nullptr;
    Ref<Material> Mat = nullptr;
    
    // Resource Identifiers (separate management)
    std::string geometryName = "";  // "cube_geometry", "sphere_geometry", etc.
    std::string materialName = "";  // "default_cube_material", "pbr_material", etc.
    
    // Resource State Tracking
    bool geometryCreated = false;
    bool materialCreated = false;
    bool geometryFailed = false;
    bool materialFailed = false;
    
    // Resource cleanup callbacks
    std::function<void()> geometryCleanup;
    std::function<void()> materialCleanup;
    
    // Helper methods
    bool IsValid() const { return geometryCreated && materialCreated && VAO && Mat; }
    bool NeedsGeometryCreation() const { return !geometryCreated && !geometryFailed && !geometryName.empty(); }
    bool NeedsMaterialCreation() const { return !materialCreated && !materialFailed && !materialName.empty(); }
    bool IsFullyCreated() const { return geometryCreated && materialCreated; }
};
```

### Scene Component Lifecycle Integration

The Scene class uses lazy resource creation with separate geometry and material specification:

```cpp
// Template specialization for RenderComponentnmjjjjkkj
template<>
void Scene::OnComponentAdded<RenderComponent>(Entity entity, RenderComponent& component) {
    // Determine geometry and material names based on entity type or explicit specification
    auto [geometryName, materialName] = DetermineResourceNames(entity, component);
    
    // Mark component for lazy resource creation (non-blocking)
    component.geometryName = geometryName;
    component.materialName = materialName;
    component.geometryCreated = false;
    component.materialCreated = false;
    component.VAO = nullptr;
    component.Mat = nullptr;
}

template<>
void Scene::OnComponentRemoved<RenderComponent>(Entity entity, RenderComponent& component) {
    // Release references to shared resources
    if (component.geometryCleanup) {
        component.geometryCleanup();
    }
    if (component.materialCleanup) {
        component.materialCleanup();
    }
}

// Helper function to determine resource names
std::pair<std::string, std::string> Scene::DetermineResourceNames(Entity entity, RenderComponent& component) {
    // Default logic - can be overridden by explicit component configuration
    if (component.geometryName.empty() || component.materialName.empty()) {
        // Determine based on entity type (e.g., CubeEntity -> "cube_geometry", "default_cube_material")
        return {"cube_geometry", "default_cube_material"};
    }
    return {component.geometryName, component.materialName};
}
```

### Prefab System Integration

The prefab system specifies separate geometry and material resources:

```cpp
class CubeEntity : public Entity {
public:
    CubeEntity(entt::entity handle, Scene* scene) : Entity(handle, scene) {
        auto& renderComp = AddComponent<RenderComponent>();
        renderComp.geometryName = "cube_geometry";     // Specific geometry resource
        renderComp.materialName = "default_cube_material"; // Specific material resource
    }
};

// Alternative: Explicit resource specification during creation
Entity CreateCubeWithCustomMaterial(Scene* scene, const std::string& materialName) {
    auto entity = scene->CreateEntity("CustomCube");
    auto& renderComp = entity.AddComponent<RenderComponent>();
    renderComp.geometryName = "cube_geometry";
    renderComp.materialName = materialName; // Custom material
    return entity;
}
```

## Data Models

### Separate Resource Registries

Geometry and material resources are managed independently using smart pointer reference counting:

```cpp
template<typename ResourceType>
class ResourceRegistry {
private:
    std::unordered_map<std::string, Ref<ResourceType>> m_Resources;
    
public:
    Ref<ResourceType> AcquireResource(const std::string& name, std::function<Ref<ResourceType>()> creator);
    void ReleaseResource(const std::string& name);
    bool IsResourceLoaded(const std::string& name);
    long GetReferenceCount(const std::string& name); // Returns shared_ptr use_count()
};

// Specialized registries
using GeometryRegistry = ResourceRegistry<VertexArray>;
using MaterialRegistry = ResourceRegistry<Material>;

class ResourceManager {
private:
    GeometryRegistry m_GeometryRegistry;
    MaterialRegistry m_MaterialRegistry;
    
public:
    Ref<VertexArray> GetOrCreateGeometry(const std::string& name);
    Ref<Material> GetOrCreateMaterial(const std::string& name);
    void ReleaseGeometry(const std::string& name);
    void ReleaseMaterial(const std::string& name);
};
```

### Resource Creation Specifications

Different resource types have creation specifications:

```cpp
struct ResourceSpec {
    std::string type;
    std::function<Ref<VertexArray>()> createVAO;
    std::function<Ref<Material>()> createMaterial;
};

class ResourceSpecRegistry {
public:
    void RegisterSpec(const std::string& type, const ResourceSpec& spec);
    ResourceSpec GetSpec(const std::string& type);
};
```

## Error Handling

### Lazy Resource Creation

To avoid blocking during entity creation:
- Component addition is immediate and non-blocking
- Resources remain null until first render attempt
- RendererLayer checks resource validity during render pass
- GPU resources are created just-in-time during rendering

### Render-Time Resource Validation

During each render frame, geometry and materials are created independently:
```cpp
void RendererLayer::RenderEntity(Entity entity) {
    auto& renderComp = entity.GetComponent<RenderComponent>();
    
    // Create geometry if needed
    if (renderComp.NeedsGeometryCreation()) {
        renderComp.VAO = m_ResourceManager.GetOrCreateGeometry(renderComp.geometryName);
        renderComp.geometryCreated = (renderComp.VAO != nullptr);
        renderComp.geometryFailed = !renderComp.geometryCreated;
    }
    
    // Create material if needed
    if (renderComp.NeedsMaterialCreation()) {
        renderComp.Mat = m_ResourceManager.GetOrCreateMaterial(renderComp.materialName);
        renderComp.materialCreated = (renderComp.Mat != nullptr);
        renderComp.materialFailed = !renderComp.materialCreated;
    }
    
    // Only render if both resources are valid
    if (renderComp.IsValid()) {
        // Perform actual rendering with VAO and Material
        RenderWithResources(renderComp.VAO, renderComp.Mat, entity.GetComponent<TransformComponent>());
    }
}
```

### Resource Creation Failures

When GPU resource creation fails:
- Log detailed error messages with context
- Set component to a "failed" state with null resources
- Provide fallback rendering path or skip rendering for failed components

### Memory Management

Resource cleanup follows RAII principles:
- Shared resources use reference counting
- Component destruction automatically decrements reference counts
- Last reference triggers actual GPU resource cleanup

## Testing Strategy

### Unit Tests

1. **Component Lifecycle Tests**
   - Test OnComponentAdded callback triggering
   - Test OnComponentRemoved callback triggering
   - Test callback registration and deregistration

2. **Resource Management Tests**
   - Test shared resource reference counting
   - Test resource creation and cleanup
   - Test resource registry operations

3. **Service Locator Tests**
   - Test RendererLayer registration and retrieval
   - Test behavior when RendererLayer is unavailable
   - Test deferred resource creation queue

### Integration Tests

1. **Prefab Creation Tests**
   - Test cube prefab creation with automatic resource assignment
   - Test multiple cube entities sharing resources
   - Test entity destruction and resource cleanup

2. **Scene Integration Tests**
   - Test Scene and RendererLayer coordination
   - Test resource creation during scene loading
   - Test scene cleanup and resource release

### Performance Tests

1. **Resource Sharing Efficiency**
   - Measure memory usage with shared vs individual resources
   - Test performance impact of reference counting
   - Benchmark resource creation and cleanup times

2. **Callback Overhead**
   - Measure performance impact of component lifecycle callbacks
   - Test scalability with large numbers of entities
   - Profile memory allocation patterns

## Implementation Notes

### Lazy Resource Creation Benefits

This approach provides several advantages:
1. **Non-blocking Entity Creation**: Entities can be created instantly without waiting for GPU operations
2. **Batch Resource Creation**: Multiple entities can have their resources created together during render
3. **Render Context Guarantee**: Resources are always created when OpenGL context is active
4. **Memory Efficiency**: Resources are only created for entities that actually get rendered

### Reusing SetupCube Code

The existing SetupCube code is refactored into separate geometry and material creation:

```cpp
// Extract geometry creation from SetupCube()
Ref<VertexArray> RendererLayer::CreateCubeGeometry() {
    // Reuse cube vertex data from SetupCube()
    float cubeVertices[] = { /* existing cube vertex data */ };
    unsigned int cubeIndices[] = { /* existing cube index data */ };
    
    auto vao = VertexArray::Create();
    auto vbo = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    };
    vbo->SetLayout(layout);
    vao->SetVertexBuffer(vbo);
    
    auto ibo = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));
    vao->SetIndexBuffer(ibo);
    
    return vao;
}

// Create default cube material
Ref<Material> RendererLayer::CreateDefaultCubeMaterial() {
    std::string cubeShaderPath = GetShaderPath("cube.glsl");
    auto shader = Shader::Create(cubeShaderPath);
    
    auto material = Material::Create(shader);
    // Set default material properties
    return material;
}

// Resource manager integration
void RendererLayer::RegisterDefaultResources() {
    // Register geometry creators
    m_ResourceManager.RegisterGeometryCreator("cube_geometry", 
        [this]() { return CreateCubeGeometry(); });
    
    // Register material creators  
    m_ResourceManager.RegisterMaterialCreator("default_cube_material",
        [this]() { return CreateDefaultCubeMaterial(); });
}
```

### Render Loop Integration

The render loop handles separate geometry and material creation:

```cpp
void RendererLayer::OnUpdate() {
    // ... existing render setup ...
    
    // Iterate through all entities with RenderComponents
    auto view = m_Scene->GetRegistry().view<RenderComponent, TransformComponent>();
    for (auto entity : view) {
        auto& renderComp = view.get<RenderComponent>(entity);
        
        // Lazy creation of geometry resources
        if (renderComp.NeedsGeometryCreation()) {
            renderComp.VAO = m_ResourceManager.GetOrCreateGeometry(renderComp.geometryName);
            renderComp.geometryCreated = (renderComp.VAO != nullptr);
            if (!renderComp.geometryCreated) {
                renderComp.geometryFailed = true;
                ENGINE_CORE_ERROR("Failed to create geometry: {}", renderComp.geometryName);
            }
        }
        
        // Lazy creation of material resources
        if (renderComp.NeedsMaterialCreation()) {
            renderComp.Mat = m_ResourceManager.GetOrCreateMaterial(renderComp.materialName);
            renderComp.materialCreated = (renderComp.Mat != nullptr);
            if (!renderComp.materialCreated) {
                renderComp.materialFailed = true;
                ENGINE_CORE_ERROR("Failed to create material: {}", renderComp.materialName);
            }
        }
        
        // Render only if both resources are valid
        if (renderComp.IsValid()) {
            auto& transform = view.get<TransformComponent>(entity);
            RenderEntityWithResources(renderComp, transform);
        }
    }
}
```

### Performance Considerations

- Resource creation happens during render when OpenGL context is guaranteed active
- Shared resources (like cube geometry) are created once and reused
- Failed creation attempts are cached to avoid repeated failures
- No blocking operations during entity creation phase

### Extensibility

The design supports future component types:
- Template specialization allows custom lifecycle handling per component type
- Resource specification registry enables new resource types
- Lazy creation pattern works for any resource-dependent component