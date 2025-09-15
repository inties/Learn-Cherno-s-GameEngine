# Design Document

## Overview

This design implements editor enhancements for the GameEngine EditorLayer, focusing on two main features: a pause/play control system and a visual component attachment system. The pause system provides runtime control over game logic execution, while the component system offers a user-friendly interface for managing ECS components and C++ scripts through the Inspector panel.

The design leverages the existing ImGui-based editor interface and integrates seamlessly with the current ECS architecture using EnTT. The implementation maintains the existing code structure while adding new UI panels and component management functionality.

## Architecture

### Pause/Play Control System

The pause system is built around the existing `running` boolean variable in EditorLayer:

```cpp
class EditorLayer : public Layer {
private:
    bool running = false;  // Already exists
    
    // New methods for pause control
    void DrawPlaybackControls();
    void SetPaused(bool paused);
    bool IsPaused() const { return !running; }
    
    // Configuration persistence
    void SaveEditorConfig();
    void LoadEditorConfig();
};
```

The pause state affects script execution in the OnUpdate method:

```cpp
void EditorLayer::OnUpdate() {
    if (!running) return; // Skip all game logic when paused
    
    // Existing script execution code
    auto& registry = m_Scene->GetRegistry();
    auto view = registry.view<NativeScriptableComponent>();
    for (auto& [entity, nsc] : view.each()) {
        if (!nsc.Instance) {
            nsc.Instance = nsc.Initilize();
        }
        nsc.Instance->OnUpdate();
    }
}
```

### Component Attachment System

The component system extends the Inspector panel with dynamic component management:

```cpp
class EditorLayer {
private:
    // Component management state
    bool m_ShowAddComponentMenu = false;
    std::vector<std::string> m_AvailableComponents;
    std::vector<std::string> m_AvailableScripts;
    
    // New Inspector methods
    void DrawInspectorPanel() override;
    void DrawComponentAddMenu();
    void DrawComponentProperties(Entity entity);
    void AddComponentToEntity(Entity entity, const std::string& componentType);
    void RemoveComponentFromEntity(Entity entity, const std::string& componentType);
    
    // Component type registration
    void RegisterAvailableComponents();
    void RegisterAvailableScripts();
};
```

### Enhanced Hierarchy Panel

The Hierarchy panel displays all entities in the scene and allows selection:

```cpp
void EditorLayer::DrawHierarchyPanel() {
    if (ImGui::Begin("Hierarchy")) {
        if (m_Scene) {
            ImGui::Text("Scene Entities");
            ImGui::Separator();
            
            auto entities = m_Scene->GetAllEntities();
            Entity selectedEntity = m_Scene->GetSelectedEntity();
            
            if (entities.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                ImGui::Text("No entities in scene");
                ImGui::Text("Create entities through code or drag assets here");
                ImGui::PopStyleColor();
            } else {
                for (const auto& entity : entities) {
                    // Get display name
                    std::string displayName = "Entity " + std::to_string((uint32_t)entity);
                    if (entity.HasComponent<TagComponent>()) {
                        auto& tag = entity.GetComponent<TagComponent>();
                        displayName = tag.Name + " (" + std::to_string((uint32_t)entity) + ")";
                    }
                    
                    // Check if this entity is selected
                    bool isSelected = (selectedEntity.IsValid() && selectedEntity == entity);
                    
                    // Display entity as selectable item
                    if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                        m_Scene->SetSelectedEntity(entity);
                    }
                    
                    // Right-click context menu
                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Delete Entity")) {
                            m_Scene->DestroyEntity(entity);
                            if (isSelected) {
                                m_Scene->ClearSelection();
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
            }
            
            // Accept drag and drop for creating entities from assets
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                    const char* assetPath = (const char*)payload->Data;
                    Entity newEntity = m_Scene->CreateEntityFromAsset(assetPath);
                    m_Scene->SetSelectedEntity(newEntity);
                }
                ImGui::EndDragDropTarget();
            }
        } else {
            ImGui::Text("No scene loaded");
        }
    }
    ImGui::End();
}
```

### Component Type Registry

A registry system manages available component types and their UI representations:

```cpp
struct ComponentTypeInfo {
    std::string name;
    std::string displayName;
    std::function<void(Entity)> addFunction;
    std::function<void(Entity)> removeFunction;
    std::function<bool(Entity)> hasFunction;
    std::function<void(Entity)> drawPropertiesFunction;
};

class ComponentRegistry {
public:
    static void RegisterComponent(const ComponentTypeInfo& info);
    static std::vector<ComponentTypeInfo> GetAvailableComponents();
    static ComponentTypeInfo* GetComponentInfo(const std::string& name);
    
private:
    static std::unordered_map<std::string, ComponentTypeInfo> s_Components;
};
```

## Components and Interfaces

### Enhanced Inspector Panel

The Inspector panel is redesigned to support dynamic component management for Entities:

```cpp
void EditorLayer::DrawInspectorPanel() {
    if (ImGui::Begin("Inspector")) {
        if (m_Scene) {
            Entity selectedEntity = m_Scene->GetSelectedEntity();
            if (selectedEntity.IsValid()) {
                // Display Entity ID and basic info
                ImGui::Text("Entity ID: %u", (uint32_t)selectedEntity);
                
                // Display entity name if it has TagComponent
                if (selectedEntity.HasComponent<TagComponent>()) {
                    auto& tag = selectedEntity.GetComponent<TagComponent>();
                    ImGui::Text("Name: %s", tag.Name.c_str());
                }
                
                ImGui::Separator();
                
                // Draw existing components
                DrawExistingComponents(selectedEntity);
                
                ImGui::Separator();
                
                // Add Component button and menu
                DrawAddComponentSection(selectedEntity);
            } else {
                ImGui::Text("No entity selected.");
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select an entity in the Hierarchy to view its components.");
            }
        }
    }
    ImGui::End();
}

void EditorLayer::DrawAddComponentSection(Entity entity) {
    if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
        m_ShowAddComponentMenu = true;
    }
    
    if (m_ShowAddComponentMenu) {
        DrawComponentAddMenu(entity);
    }
}

void EditorLayer::DrawComponentAddMenu(Entity entity) {
    if (ImGui::BeginPopup("AddComponentMenu")) {
        ImGui::Text("Select Component Type:");
        ImGui::Separator();
        
        // Built-in components
        if (ImGui::BeginMenu("Components")) {
            for (const auto& componentName : m_AvailableComponents) {
                if (ImGui::MenuItem(componentName.c_str())) {
                    AddComponentToEntity(entity, componentName);
                    m_ShowAddComponentMenu = false;
                }
            }
            ImGui::EndMenu();
        }
        
        // Script components
        if (ImGui::BeginMenu("Scripts")) {
            for (const auto& scriptName : m_AvailableScripts) {
                if (ImGui::MenuItem(scriptName.c_str())) {
                    AddScriptToEntity(entity, scriptName);
                    m_ShowAddComponentMenu = false;
                }
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndPopup();
    }
    
    if (m_ShowAddComponentMenu) {
        ImGui::OpenPopup("AddComponentMenu");
    }
}
```

### Component Property Editors

Each component type has a dedicated property editor:

```cpp
void EditorLayer::DrawTransformComponent(Entity entity) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("Translation", &transform.Translation.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.Rotation.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform.Scale.x, 0.1f);
            
            // Remove component button
            if (ImGui::Button("Remove##Transform")) {
                entity.RemoveComponent<TransformComponent>();
            }
        }
    }
}

void EditorLayer::DrawTagComponent(Entity entity) {
    if (entity.HasComponent<TagComponent>()) {
        auto& tag = entity.GetComponent<TagComponent>();
        
        if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
            char buffer[256];
            strcpy(buffer, tag.Name.c_str());
            if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                tag.Name = std::string(buffer);
            }
            
            if (ImGui::Button("Remove##Tag")) {
                entity.RemoveComponent<TagComponent>();
            }
        }
    }
}

void EditorLayer::DrawRenderComponent(Entity entity) {
    if (entity.HasComponent<RenderComponent>()) {
        auto& render = entity.GetComponent<RenderComponent>();
        
        if (ImGui::CollapsingHeader("Render Component", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Geometry name
            char geoBuffer[256];
            strcpy(geoBuffer, render.GeoMetryName.c_str());
            if (ImGui::InputText("Geometry", geoBuffer, sizeof(geoBuffer))) {
                render.GeoMetryName = std::string(geoBuffer);
            }
            
            // Material name
            char matBuffer[256];
            strcpy(matBuffer, render.MatName.c_str());
            if (ImGui::InputText("Material", matBuffer, sizeof(matBuffer))) {
                render.MatName = std::string(matBuffer);
            }
            
            // Status display
            ImGui::Text("Geometry Created: %s", render.geometryCreated ? "Yes" : "No");
            ImGui::Text("Material Created: %s", render.materialCreated ? "Yes" : "No");
            
            if (ImGui::Button("Remove##Render")) {
                entity.RemoveComponent<RenderComponent>();
            }
        }
    }
}
```

### Script Component Management

Script components require special handling for C++ class binding:

```cpp
void EditorLayer::DrawNativeScriptComponent(Entity entity) {
    if (entity.HasComponent<NativeScriptableComponent>()) {
        auto& script = entity.GetComponent<NativeScriptableComponent>();
        
        if (ImGui::CollapsingHeader("Native Script", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Display script type information
            if (script.Instance) {
                ImGui::Text("Script Type: %s", typeid(*script.Instance).name());
                ImGui::Text("Status: Active");
            } else {
                ImGui::Text("Script Type: Not Initialized");
                ImGui::Text("Status: Inactive");
            }
            
            // Script control buttons
            if (ImGui::Button("Reinitialize Script")) {
                if (script.DestroyScript && script.Instance) {
                    script.DestroyScript(script.Instance);
                }
                script.Instance = script.Initilize();
            }
            
            if (ImGui::Button("Remove##Script")) {
                if (script.DestroyScript && script.Instance) {
                    script.DestroyScript(script.Instance);
                }
                entity.RemoveComponent<NativeScriptableComponent>();
            }
        }
    }
}

void EditorLayer::AddScriptToEntity(Entity entity, const std::string& scriptName) {
    if (entity.HasComponent<NativeScriptableComponent>()) {
        ENGINE_CORE_WARN("Entity already has a script component");
        return;
    }
    
    auto& script = entity.AddComponent<NativeScriptableComponent>();
    
    // Bind script based on name
    if (scriptName == "MoveScript") {
        script.Bind<MoveScript>(entity);
    }
    // Add more script types as they become available
    
    ENGINE_CORE_INFO("Added script component: {}", scriptName);
}
```

## Data Models

### Editor Configuration

The editor state is persisted using a simple configuration system:

```cpp
struct EditorConfig {
    bool isPaused = true;  // Default to paused for safety
    std::string lastSelectedObject = "";
    glm::vec2 viewportSize = {800, 600};
    
    // Serialization methods
    void SaveToFile(const std::string& filepath);
    bool LoadFromFile(const std::string& filepath);
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& json);
};

class EditorLayer {
private:
    EditorConfig m_Config;
    std::string m_ConfigPath = ".kiro/editor_config.json";
    
    void SaveEditorConfig() {
        m_Config.isPaused = !running;
        m_Config.SaveToFile(m_ConfigPath);
    }
    
    void LoadEditorConfig() {
        if (m_Config.LoadFromFile(m_ConfigPath)) {
            running = !m_Config.isPaused;
        }
    }
};
```

### Component Type Registration

Available components are registered at startup:

```cpp
void EditorLayer::RegisterAvailableComponents() {
    m_AvailableComponents.clear();
    
    // Register built-in components
    m_AvailableComponents.push_back("Transform");
    m_AvailableComponents.push_back("Tag");
    m_AvailableComponents.push_back("Render");
    
    // Future: Dynamic registration from reflection system
}

void EditorLayer::RegisterAvailableScripts() {
    m_AvailableScripts.clear();
    
    // Register available script classes
    m_AvailableScripts.push_back("MoveScript");
    
    // Future: Automatic discovery through reflection or registration macros
}
```

## Error Handling

### Component Addition Validation

The system validates component additions to prevent errors:

```cpp
void EditorLayer::AddComponentToEntity(Entity entity, const std::string& componentType) {
    try {
        if (componentType == "Transform") {
            if (entity.HasComponent<TransformComponent>()) {
                ENGINE_CORE_WARN("Entity already has Transform component");
                return;
            }
            entity.AddComponent<TransformComponent>();
        }
        else if (componentType == "Tag") {
            if (entity.HasComponent<TagComponent>()) {
                ENGINE_CORE_WARN("Entity already has Tag component");
                return;
            }
            entity.AddComponent<TagComponent>("New Entity");
        }
        else if (componentType == "Render") {
            if (entity.HasComponent<RenderComponent>()) {
                ENGINE_CORE_WARN("Entity already has Render component");
                return;
            }
            entity.AddComponent<RenderComponent>();
        }
        
        ENGINE_CORE_INFO("Added {} component to entity", componentType);
    }
    catch (const std::exception& e) {
        ENGINE_CORE_ERROR("Failed to add {} component: {}", componentType, e.what());
    }
}
```

### Configuration Persistence

Configuration loading includes error handling:

```cpp
bool EditorConfig::LoadFromFile(const std::string& filepath) {
    try {
        if (!std::filesystem::exists(filepath)) {
            ENGINE_CORE_INFO("Editor config file not found, using defaults");
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ENGINE_CORE_ERROR("Failed to open editor config file: {}", filepath);
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        FromJson(json);
        
        ENGINE_CORE_INFO("Loaded editor configuration from {}", filepath);
        return true;
    }
    catch (const std::exception& e) {
        ENGINE_CORE_ERROR("Failed to load editor config: {}", e.what());
        return false;
    }
}
```

## Testing Strategy

### Unit Tests

1. **Pause/Play System Tests**
   - Test pause state persistence across sessions
   - Test script execution control with pause state
   - Test UI button state synchronization

2. **Component Management Tests**
   - Test component addition and removal
   - Test duplicate component prevention
   - Test component property editing

3. **Configuration System Tests**
   - Test configuration saving and loading
   - Test default configuration handling
   - Test configuration file corruption recovery

### Integration Tests

1. **Editor Panel Integration**
   - Test Inspector panel component display
   - Test component menu functionality
   - Test property editor updates

2. **ECS Integration**
   - Test Entity-GameObject conversion
   - Test component lifecycle with editor operations
   - Test script component binding and execution

### Manual Testing

1. **User Workflow Tests**
   - Test complete component addition workflow
   - Test pause/play during script execution
   - Test configuration persistence across editor restarts

2. **UI Responsiveness Tests**
   - Test component menu responsiveness
   - Test property editor real-time updates
   - Test pause button visual feedback

## Implementation Notes

### Entity Selection System

The Scene class manages entity selection directly:

```cpp
class Scene {
private:
    Entity m_SelectedEntity = {}; // Invalid entity by default
    
public:
    void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
    Entity GetSelectedEntity() const { return m_SelectedEntity; }
    void ClearSelection() { m_SelectedEntity = {}; }
    
    // Get all entities in the scene for hierarchy display
    std::vector<Entity> GetAllEntities() const {
        std::vector<Entity> entities;
        auto view = m_Registry.view<entt::entity>();
        for (auto entity : view) {
            entities.emplace_back(entity, const_cast<Scene*>(this));
        }
        return entities;
    }
};
```

### Playback Controls UI

The playback controls are integrated into the main editor toolbar:

```cpp
void EditorLayer::DrawPlaybackControls() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
    
    // Play/Pause button with icon
    const char* buttonText = running ? "⏸ Pause" : "▶ Play";
    ImVec4 buttonColor = running ? ImVec4(1.0f, 0.6f, 0.0f, 1.0f) : ImVec4(0.0f, 0.8f, 0.0f, 1.0f);
    
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    if (ImGui::Button(buttonText, ImVec2(100, 0))) {
        SetPaused(running); // Toggle pause state
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // Status indicator
    const char* statusText = running ? "Running" : "Paused";
    ImVec4 statusColor = running ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::TextColored(statusColor, "Status: %s", statusText);
    
    ImGui::PopStyleVar();
}
```

### Component Registration Extensibility

The design supports future expansion through a registration system:

```cpp
// Future: Macro-based component registration
#define REGISTER_COMPONENT(ComponentType, DisplayName) \
    ComponentRegistry::RegisterComponent({ \
        #ComponentType, \
        DisplayName, \
        [](Entity e) { e.AddComponent<ComponentType>(); }, \
        [](Entity e) { e.RemoveComponent<ComponentType>(); }, \
        [](Entity e) { return e.HasComponent<ComponentType>(); }, \
        [](Entity e) { DrawComponentProperties<ComponentType>(e); } \
    });

// Usage in initialization
void EditorLayer::RegisterComponents() {
    REGISTER_COMPONENT(TransformComponent, "Transform");
    REGISTER_COMPONENT(TagComponent, "Tag");
    REGISTER_COMPONENT(RenderComponent, "Render Component");
}
```

This design provides a solid foundation for editor enhancements while maintaining compatibility with the existing codebase and allowing for future extensibility.