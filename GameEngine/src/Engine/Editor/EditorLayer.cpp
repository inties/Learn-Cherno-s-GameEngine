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
        // ��ʼ�������������ݸ���Ⱦ��
        m_Scene = CreateRef<Scene>();
        if (m_RendererLayer) {
            m_RendererLayer->SetScene(m_Scene);
        }
        
        // ��ѡ������һ��Ĭ�ϵ���Ŀ�����Ե�ǰ����Ŀ¼Ϊ����
        // ʵ����Ŀ�п�ͨ��UI����������
        // ProjectManager::Get()->SetProjectRoot(std::filesystem::current_path().string());
        
        // ��ʼ���ʲ��б�ɨ����Ŀ��Ŀ¼�µ������ļ����ļ���
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
            // ��Ŀ��·������
            static char rootBuf[512] = {0};
            if (rootBuf[0] == '\0') {
                // ������ʾʱ��䵱ǰ����
                std::string cur = pm->GetProjectRoot();
                strncpy(rootBuf, cur.c_str(), sizeof(rootBuf) - 1);
            }
            ImGui::Text("Project Root:");
            ImGui::SameLine();
            ImGui::InputText("##projroot", rootBuf, sizeof(rootBuf));
            ImGui::SameLine();
            if (ImGui::Button("Set Root")) {
                pm->SetProjectRoot(rootBuf);
                pm->RefreshAssets(); // ˢ���ʲ��б�
            }

            ImGui::Separator();

            // ͨ������·������ļ�/�ļ���
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
                     addFileBuf[0] = '\0'; // ��������
                     showSuccess = true;
                     showError = false;
                 } else {
                     showError = true;
                     showSuccess = false;
                 }
             }
             
             // ��ʾ״̬��Ϣ
             if (showSuccess) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // ��ɫ
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             } else if (showError) {
                 ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // ��ɫ
                 ImGui::Text("%s", statusMessage.c_str());
                 ImGui::PopStyleColor();
             }

            // // ��ͳ���ļ���������
            // if (ImGui::Button("Browse File")) {
            //     pm->AddFileByBrowse();
            // }

            // ˵���ı�
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
                // �п���ק
                ImGui::Selectable((label + "##" + std::to_string(idx)).c_str(), false);
                if (ImGui::BeginDragDropSource()) {
                    // ����ģ��������ק����ʵ�����������ͺ�����չ
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
            // ���ܴ� Content ��ק��ģ��·��������ʵ��
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadAssetPath)) {
                    const char* rel = (const char*)payload->Data;
                    if (m_Scene) {
                        m_Scene->AddModelInstance(rel, glm::mat4(1.0f));
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // �г���������
            if (m_Scene) {
                int i = 0;
                for (const auto& obj : m_Scene->GetModelObjects()) {
                    // ����ʾ�ļ���
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

                // ���մ� Content ��ק��ģ��·��
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
        // ������DockSpace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // ����ʹ��ImGuiWindowFlags_NoDocking��־ʹ�����ڲ���ͣ������������
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

        // ��ʹ��ImGuiDockNodeFlags_PassthruCentralNodeʱ��DockSpace()����Ⱦ���ǵı���
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

            // ����Ĭ��ͣ�����֣����ڵ�һ������ʱ��
            static bool first_time = true;
            if (first_time) {
                first_time = false;
                
                ImGui::DockBuilderRemoveNode(dockspace_id); // ����κ����в���
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // ��ӿյ�dockspace�ڵ�
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

                // �ָ�dockspaceΪ�������
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

                // ������ͣ����ָ��λ��
                ImGui::DockBuilderDockWindow("Content", dock_id_left);
                ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Console", dock_id_down);
                ImGui::DockBuilderDockWindow("Viewport", dockspace_id); // ��������
                
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

        // ���Ƹ������
        DrawContentPanel();
        DrawHierarchyPanel();
        DrawInspectorPanel();
        DrawViewportPanel();
        DrawConsolePanel();
    }

    void EditorLayer::OnEvent(Event& event) {
        // �¼������߼�
        EventDispatcher dispatcher(event);
       /* dispatcher.Dispatch<FileDragDropEvent>([this](FileDragDropEvent& e) {
            return OnFileDragDrop(e);
        });*/
    }
}