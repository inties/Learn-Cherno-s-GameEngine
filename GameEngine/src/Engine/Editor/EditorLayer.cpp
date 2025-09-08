#include "pch.h"
#include "EditorLayer.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Engine/Application.h"
#include "Engine/RendererLayer.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/log.h"
#include <filesystem>
#include <cstring>

namespace Engine {

    static const char* kPayloadAssetPath = "ASSET_PATH";

    EditorLayer::EditorLayer()
        : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {
        FindRendererLayer();
        // Create scene and set up renderer
        m_Scene = CreateRef<Scene>();
        if (m_RendererLayer) {
            m_RendererLayer->SetScene(m_Scene);
        }
        
        // Initialize project manager and set up asset management
        // TODO: Set up UI for project root configuration
        // ProjectManager::Get()->SetProjectRoot(std::filesystem::current_path().string());
        
        // Refresh assets to load available files
        ProjectManager::Get()->RefreshAssets();
    }

    void EditorLayer::FindRendererLayer() {
        // Access layer stack via Application getter
        auto& layers = Application::Get().GetLayerStack().m_Layers;
        for (auto* layer : layers) {
            auto* rl = dynamic_cast<RendererLayer*>(layer);
            if (rl) { m_RendererLayer = rl; break; }
        }
    }

    bool EditorLayer::OnMouseMove(MouseMoveEvent& event)
    {
        auto& [x,y] = event.getMousePos();
        m_mousePos.x = x;
        m_mousePos.y = y;
        return true;
    }

    void EditorLayer::DrawContentPanel() {
        auto pm = ProjectManager::Get();
        if (ImGui::Begin("Content")) {
            // Project root configuration
            static char rootBuf[512] = {0};
            if (rootBuf[0] == '\0') {
                // Initialize with current project root
                std::string cur = pm->GetProjectRoot();
                strncpy(rootBuf, cur.c_str(), sizeof(rootBuf) - 1);
            }
            ImGui::Text("Project Root:");
            ImGui::SameLine();
            ImGui::InputText("##projroot", rootBuf, sizeof(rootBuf));
            ImGui::SameLine();
            if (ImGui::Button("Set Root")) {
                pm->SetProjectRoot(rootBuf);
                pm->RefreshAssets(); // Refresh asset list
            }

            ImGui::Separator();

            // Add file/folder functionality
            static char addFileBuf[512] = {0};
            ImGui::Text("Add File/Folder:");
            ImGui::InputText("##addpath", addFileBuf, sizeof(addFileBuf));
            ImGui::SameLine();
            static std::string statusMessage = "";
             static bool showSuccess = false;
             static bool showError = false;
             
             if (ImGui::Button("Add")) {
                 std::string inputPath(addFileBuf);
                 auto result = pm->ProcessInputPath(inputPath);
                 
                 statusMessage = result.message;
                 if (result.success) {
                     addFileBuf[0] = '\0'; // Clear input buffer
                     showSuccess = true;
                     showError = false;
                 } else {
                     showError = true;
                     showSuccess = false;
                 }
             }
             
             // Status message display
             if (showSuccess) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             } else if (showError) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             }

            // // TODO: File browser functionality
            // if (ImGui::Button("Browse File")) {
            //     pm->AddFileByBrowse();
            // }

            // Help text
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::Text("Note: Use absolute path input above to add files and folders to the project");
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::Text("Assets:");
            ImGui::BeginChild("##content_list", ImVec2(0, 0), true);
            
            // Debug: Check if we have any assets
            auto assets = pm->ListAssets();
            //ENGINE_CORE_INFO("Content panel: Found {} assets", assets.size());
            if (assets.empty()) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No assets found! Check project root and refresh.");
            }
            
            int idx = 0;
            for (const auto& asset : assets) {
                std::string label = asset.relativePath;
                // Asset item - make it selectable and draggable
                if (ImGui::Selectable((label + "##" + std::to_string(idx)).c_str(), false)) {
                    ENGINE_CORE_INFO("Selected asset: {}", label);
                }
                
                // Set up drag and drop source - need to check if item is being dragged
                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        // Set up drag and drop payload
                        std::string text = label;
                        ImGui::TextUnformatted(text.c_str());
                        ImGui::SetDragDropPayload(kPayloadAssetPath, text.c_str(), (text.size() + 1) * sizeof(char));
                        //ENGINE_CORE_INFO("Started dragging asset: {}", text);
                        ImGui::EndDragDropSource();
                    }
                } else {
                    // Check if we're hovering over the item
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Drag this asset to Hierarchy or Viewport");
                    }
                }
                idx++;
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void EditorLayer::DrawHierarchyPanel() {
        if (ImGui::Begin("Hierarchy")) {
            // Create an invisible button that covers the entire window content area for drag and drop
            ImVec2 windowSize = ImGui::GetContentRegionAvail();
            // Only create invisible button if window has valid size to prevent assertion
            if (windowSize.x > 0.0f && windowSize.y > 0.0f) {
                ImGui::InvisibleButton("##hierarchy_drag_area", windowSize);
                
                // Accept drag and drop from Content panel - covers entire window
                if (ImGui::BeginDragDropTarget()) {
                ENGINE_CORE_INFO("Hierarchy panel is ready to accept drag and drop");
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadAssetPath)) {
                    const char* rel = (const char*)payload->Data;
                    ENGINE_CORE_INFO("Accepted drag drop payload: {}", rel);
                    if (m_Scene) {
                        ENGINE_CORE_INFO("Creating game object from dragged asset: {}", rel);
                        m_Scene->CreateGameObject(rel, glm::mat4(1.0f));
                    } else {
                        ENGINE_CORE_ERROR("Scene is null, cannot create game object");
                    }
                } else {
                    // Check if there's any payload at all
                    if (ImGui::IsDragDropPayloadBeingAccepted()) {
                        ENGINE_CORE_INFO("Drag drop payload is being accepted but not our type");
                    }
                }
                ImGui::EndDragDropTarget();
                }
                
                // Reset cursor position to draw content over the invisible button
                ImGui::SetCursorPos(ImVec2(0, 0));
            }
            
            ImGui::Text("Scene");
            ImGui::Separator();

            // Display scene game objects
            if (m_Scene) {
                int i = 0;
                int selectedIndex = m_Scene->GetSelectedObjectIndex();
                for (const auto& obj : m_Scene->GetGameObjects()) {
                    // Get display name
                    std::string name = std::filesystem::path(obj.modelPath).filename().string();
                    if (name.empty()) name = obj.modelPath;
                    
                    // Check if this object is selected
                    bool isSelected = (i == selectedIndex);
                    
                    // Display different icons and colors based on loading status
                    if (obj.isLoading) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow for loading
                        if (ImGui::Selectable((name + " (Loading...)##obj" + std::to_string(i)).c_str(), isSelected)) {
                            m_Scene->SetSelectedObject(i);
                        }
                        ImGui::PopStyleColor();
                    } else if (obj.loadFailed) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red for failed
                        if (ImGui::Selectable((name + " (Failed)##obj" + std::to_string(i)).c_str(), isSelected)) {
                            m_Scene->SetSelectedObject(i);
                        }
                        ImGui::PopStyleColor();
                    } else if (auto model = obj.model.lock()) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green for success
                        if (ImGui::Selectable((name + "##obj" + std::to_string(i)).c_str(), isSelected)) {
                            m_Scene->SetSelectedObject(i);
                        }
                        ImGui::PopStyleColor();
                    } else {
                        // Model object is empty (weak pointer may be invalid)
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f)); // Gray
                        if (ImGui::Selectable((name + " (Empty)##obj" + std::to_string(i)).c_str(), isSelected)) {
                            m_Scene->SetSelectedObject(i);
                        }
                        ImGui::PopStyleColor();
                    }
                    
                    i++;
                }
                
                if (m_Scene->GetGameObjects().empty()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                    ImGui::Text("Drag model files here to create game objects");
                    ImGui::PopStyleColor();
                }
            } else {
                ImGui::TextUnformatted("[No Scene]");
            }
        }
        ImGui::End();
    }

    void EditorLayer::DrawInspectorPanel() {
        if (ImGui::Begin("Inspector")) {
            if (m_Scene) {
                const GameObject* selectedObj = m_Scene->GetSelectedObject();
                if (selectedObj) {
                    // 显示选中对象的信息
                    std::string name = std::filesystem::path(selectedObj->modelPath).filename().string();
                    if (name.empty()) name = selectedObj->modelPath;
                    
                    ImGui::Text("Selected Object: %s", name.c_str());
                    ImGui::Separator();
                    
                    // 显示模型路径
                    ImGui::Text("Model Path: %s", selectedObj->modelPath.c_str());
                    
                    // 显示加载状态
                    if (selectedObj->isLoading) {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status: Loading...");
                    } else if (selectedObj->loadFailed) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: Load Failed");
                    } else if (auto model = selectedObj->model.lock()) {
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Loaded");
                    } else {
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Status: Empty");
                    }
                    
                    ImGui::Separator();
                    
                    // 显示Transform信息
                    ImGui::Text("Transform:");
                    const glm::mat4& transform = selectedObj->transform;
                    
                    // 提取位置、旋转、缩放信息（简化显示）
                    glm::vec3 position = glm::vec3(transform[3]);
                    ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
                    
                    // 显示变换矩阵（可选）
                    if (ImGui::CollapsingHeader("Transform Matrix")) {
                        for (int i = 0; i < 4; i++) {
                            ImGui::Text("[%.2f, %.2f, %.2f, %.2f]", 
                                transform[i][0], transform[i][1], transform[i][2], transform[i][3]);
                        }
                    }
                    
                } else {
                    ImGui::Text("No object selected.");
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select an object in the Hierarchy to view its properties.");
                }
            } else {
                ImGui::Text("No scene loaded.");
            }
        }
        ImGui::End();
    }

    void EditorLayer::DrawConsolePanel() {
        if (ImGui::Begin("Console")) {
            ImGui::Text("Engine Log Output:");
            ImGui::Separator();
            
            if (ImGui::Button("Clear Log")) {
                // Clear log functionality
                ENGINE_CORE_INFO("Console log cleared");
            }
            ImGui::SameLine();
            
            static bool autoScroll = true;
            ImGui::Checkbox("Auto Scroll", &autoScroll);
            
            ImGui::Separator();
            
            // Create a child window to display logs
            if (ImGui::BeginChild("LogOutput", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {
                // Display helpful information
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Log information will be displayed here during engine runtime");
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[INFO] Engine log system initialized");
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[WARN] Drag model files to Hierarchy or Viewport for loading");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[ERROR] Error messages will be displayed in red");
                
                if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void EditorLayer::DrawViewportPanel() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Viewport")) {
            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();

            // 获取viewport在整个窗口中的位置信息
            ImVec2 viewportPos = ImGui::GetWindowPos();           // 窗口位置
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin(); // 内容区域相对窗口的偏移
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax(); // 内容区域右下角
            ImVec2 viewportContentPos = ImVec2(viewportPos.x + contentMin.x, viewportPos.y + contentMin.y); // 内容区域绝对位置

            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (m_RendererLayer) {
                // Resize render target when viewport size changes (and is valid)
                if ((m_ViewportSize.x != avail.x || m_ViewportSize.y != avail.y) && avail.x > 0 && avail.y > 0) {
                    m_ViewportSize = { avail.x, avail.y };
                    m_RendererLayer->ResizeRenderTarget((unsigned int)avail.x, (unsigned int)avail.y);
                    ENGINE_CORE_INFO("offscreen FBO resized to{},{}", m_RendererLayer->GetRenderWidth(), m_RendererLayer->GetRenderHeight());
                }

                unsigned int texID = m_RendererLayer->GetRenderTextureID();
                if (texID) {
                    ImGui::Image((ImTextureID)(intptr_t)texID, avail, ImVec2(0, 1), ImVec2(1, 0));
                    
                    // 计算鼠标在viewport中的归一化位置
                    if (m_ViewportHovered) {
                        ImVec2 mousePos = ImGui::GetMousePos();
                        ImVec2 relativePos = ImVec2(
                            mousePos.x - viewportContentPos.x,
                            mousePos.y - viewportContentPos.y
                        );
                        relativePos.x=glm::clamp(relativePos.x, 0.0f, m_ViewportSize.x);
                        relativePos.y=glm::clamp(relativePos.y, 0.0f, m_ViewportSize.y);
                        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            ENGINE_CORE_INFO("mousePos:{},{}", relativePos.x , relativePos.y);
                           std::cout<< m_RendererLayer->ReadPickBuffer(relativePos.x, m_RendererLayer->GetRenderHeight()-1-relativePos.y)<<std::endl;
                        }

                        //// 归一化到[0,1]范围
                        //if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0) {
                        //    m_mousePos.x = relativePos.x / m_ViewportSize.x;
                        //    m_mousePos.y = relativePos.y / m_ViewportSize.y;
                        //    
                        //    // 确保坐标在有效范围内
                        //    m_mousePos.x = glm::clamp(m_mousePos.x, 0.0f, 1.0f);
                        //    m_mousePos.y = glm::clamp(m_mousePos.y, 0.0f, 1.0f);
                        //}
                    }
                } else {
                    ImGui::Text("Render target not ready.");
                }

                // Accept drag and drop from Content panel
                if (ImGui::BeginDragDropTarget()) {
                    ENGINE_CORE_INFO("Viewport is ready to accept drag and drop");
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadAssetPath)) {
                        const char* rel = (const char*)payload->Data;
                        ENGINE_CORE_INFO("Accepted drag drop payload in Viewport: {}", rel);
                        if (m_Scene) {
                            ENGINE_CORE_INFO("Creating game object from dragged asset in Viewport: {}", rel);
                            m_Scene->CreateGameObject(rel, glm::mat4(1.0f));
                        } else {
                            ENGINE_CORE_ERROR("Scene is null, cannot create game object");
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            } else {
                ImGui::Text("RendererLayer not found.");
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::OnUpdate()
    {
        //捕获鼠标在viewport中的位置(归一化)
        // 注意：这个函数应该在DrawViewportPanel之后调用，或者将鼠标位置计算移到DrawViewportPanel中
        
        // 方法1：使用ImGui::GetMousePos()和窗口信息（需要在ImGui渲染期间调用）
        // 方法2：使用平台相关的鼠标位置获取（推荐在这里使用）
        
        // 这里暂时留空，建议将鼠标位置计算移到DrawViewportPanel函数中
        // 因为ImGui的窗口信息只在ImGui渲染期间有效
    }

    void EditorLayer::OnImGuiRender() {
        // Setup DockSpace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // ???????ImGuiWindowFlags_NoDocking??????????????????????????
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // ?????ImGuiDockNodeFlags_PassthruCentralNode???DockSpace()?????????????
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            // Display scene game objects???????????????????
            static bool first_time = true;
            if (first_time) {
                first_time = false;
                
                ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any existing layout
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add new dockspace node
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

                // Split dockspace into sections
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
                
                // Split left panel into top and bottom for Content and Hierarchy
                auto dock_id_left_top = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.5f, nullptr, &dock_id_left);

                // Dock windows to their positions
                ImGui::DockBuilderDockWindow("Content", dock_id_left_top);
                ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Console", dock_id_down);
                ImGui::DockBuilderDockWindow("Viewport", dockspace_id); // Center viewport
                
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

        // Draw all panels
        DrawContentPanel();
        DrawHierarchyPanel();
        DrawInspectorPanel();
        DrawViewportPanel();
        DrawConsolePanel();
    }

    void EditorLayer::OnEvent(Event& event) {
        // Handle events
        EventDispatcher dispatcher(event);
        // dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent& e) {
        //     return OnMouseMove(e);
        // });
    }
}