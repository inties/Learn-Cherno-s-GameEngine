#include "pch.h"
#include "EditorLayer.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Engine/Application.h"
#include "Engine/RendererLayer.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Scene/Scene.h"
#include <filesystem>
#include <cstring>

namespace Engine {

    static const char* kPayloadAssetPath = "ASSET_PATH";

    EditorLayer::EditorLayer()
        : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {
        FindRendererLayer();
        // 初始化场景，并传递给渲染层
        m_Scene = CreateRef<Scene>();
        if (m_RendererLayer) {
            m_RendererLayer->SetScene(m_Scene);
        }
        
        // 可选：设置一个默认的项目根（以当前工作目录为例）
        // 实际项目中可通过UI输入来设置
        // ProjectManager::Get()->SetProjectRoot(std::filesystem::current_path().string());
        
        // 初始化资产列表，扫描项目根目录下的所有文件和文件夹
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

    void EditorLayer::DrawContentPanel() {
        auto pm = ProjectManager::Get();
        if (ImGui::Begin("Content")) {
            // 项目根路径设置
            static char rootBuf[512] = {0};
            if (rootBuf[0] == '\0') {
                // 初次显示时填充当前设置
                std::string cur = pm->GetProjectRoot();
                strncpy(rootBuf, cur.c_str(), sizeof(rootBuf) - 1);
            }
            ImGui::Text("Project Root:");
            ImGui::SameLine();
            ImGui::InputText("##projroot", rootBuf, sizeof(rootBuf));
            ImGui::SameLine();
            if (ImGui::Button("Set Root")) {
                pm->SetProjectRoot(rootBuf);
                pm->RefreshAssets(); // 刷新资产列表
            }

            ImGui::Separator();

            // 通过绝对路径添加文件/文件夹
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
                     addFileBuf[0] = '\0'; // 清空输入框
                     showSuccess = true;
                     showError = false;
                 } else {
                     showError = true;
                     showSuccess = false;
                 }
             }
             
             // 显示状态消息
             if (showSuccess) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // 绿色
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             } else if (showError) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 红色
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             }

            // // 传统的文件浏览器添加
            // if (ImGui::Button("Browse File")) {
            //     pm->AddFileByBrowse();
            // }

            // 说明文本
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::Text("Note: Use absolute path input above to add files and folders to the project");
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::Text("Assets:");
            ImGui::BeginChild("##content_list", ImVec2(0, 0), true);
            int idx = 0;
            for (const auto& asset : pm->ListAssets()) {
                std::string label = asset.relativePath;
                // 行可拖拽
                ImGui::Selectable((label + "##" + std::to_string(idx)).c_str(), false);
                if (ImGui::BeginDragDropSource()) {
                    // 仅对模型允许拖拽生成实例，其它类型后续扩展
                    std::string text = label;
                    ImGui::TextUnformatted(text.c_str());
                    ImGui::SetDragDropPayload(kPayloadAssetPath, text.c_str(), (text.size() + 1) * sizeof(char));
                    ImGui::EndDragDropSource();
                }
                idx++;
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void EditorLayer::DrawHierarchyPanel() {
        if (ImGui::Begin("Hierarchy")) {
            ImGui::Text("Scene");
            ImGui::Separator();
            // 接受从 Content 拖拽的模型路径，生成实例
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadAssetPath)) {
                    const char* rel = (const char*)payload->Data;
                    if (m_Scene) {
                        m_Scene->AddModelInstance(rel, glm::mat4(1.0f));
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // 列出场景对象
            if (m_Scene) {
                int i = 0;
                for (const auto& obj : m_Scene->GetModelObjects()) {
                    // 仅显示文件名
                    std::string name = std::filesystem::path(obj.modelPath).filename().string();
                    if (name.empty()) name = obj.modelPath;
                    ImGui::Selectable((name + "##obj" + std::to_string(i)).c_str());
                    i++;
                }
            } else {
                ImGui::TextUnformatted("[No Scene]");
            }
        }
        ImGui::End();
    }

    void EditorLayer::DrawInspectorPanel() {
        if (ImGui::Begin("Inspector")) {
            ImGui::Text("Select an entity to see its properties.");
            ImGui::Separator();
            ImGui::Text("[M1 placeholder]");
        }
        ImGui::End();
    }

    void EditorLayer::DrawConsolePanel() {
        if (ImGui::Begin("Console")) {
            ImGui::TextWrapped("Editor console output will appear here. [M1 placeholder]");
        }
        ImGui::End();
    }

    void EditorLayer::DrawViewportPanel() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Viewport")) {
            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();

            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (m_RendererLayer) {
                // Resize render target when viewport size changes (and is valid)
                if ((m_ViewportSize.x != avail.x || m_ViewportSize.y != avail.y) && avail.x > 0 && avail.y > 0) {
                    m_ViewportSize = { avail.x, avail.y };
                    m_RendererLayer->ResizeRenderTarget((unsigned int)avail.x, (unsigned int)avail.y);
                }

                unsigned int texID = m_RendererLayer->GetRenderTextureID();
                if (texID) {
                    ImGui::Image((ImTextureID)(intptr_t)texID, avail, ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    ImGui::Text("Render target not ready.");
                }

                // 接收从 Content 拖拽的模型路径
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadAssetPath)) {
                        const char* rel = (const char*)payload->Data;
                        if (m_Scene) {
                            m_Scene->AddModelInstance(rel, glm::mat4(1.0f));
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

    void EditorLayer::OnImGuiRender() {
        // 创建主DockSpace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // 我们使用ImGuiWindowFlags_NoDocking标志使父窗口不可停靠到其他窗口
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

        // 当使用ImGuiDockNodeFlags_PassthruCentralNode时，DockSpace()将渲染我们的背景
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

            // 设置默认停靠布局（仅在第一次运行时）
            static bool first_time = true;
            if (first_time) {
                first_time = false;
                
                ImGui::DockBuilderRemoveNode(dockspace_id); // 清除任何现有布局
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // 添加空的dockspace节点
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

                // 分割dockspace为多个区域
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

                // 将窗口停靠到指定位置
                ImGui::DockBuilderDockWindow("Content", dock_id_left);
                ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Console", dock_id_down);
                ImGui::DockBuilderDockWindow("Viewport", dockspace_id); // 中央区域
                
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

        // 绘制各个面板
        DrawContentPanel();
        DrawHierarchyPanel();
        DrawInspectorPanel();
        DrawViewportPanel();
        DrawConsolePanel();
    }

    void EditorLayer::OnEvent(Event& event) {
        // 事件处理逻辑
        EventDispatcher dispatcher(event);
       /* dispatcher.Dispatch<FileDragDropEvent>([this](FileDragDropEvent& e) {
            return OnFileDragDrop(e);
        });*/
    }
}