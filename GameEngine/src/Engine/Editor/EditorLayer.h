#pragma once

#include "Engine/Layer.h"
#include "Engine/core.h"
#include <glm/glm.hpp>

namespace Engine {
    class RendererLayer;
    class Scene;

    class ENGINE_API EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override {}
        virtual void OnUpdate() override {}
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& event) override;

    private:
        void DrawHierarchyPanel();
        void DrawInspectorPanel();
        void DrawConsolePanel();
        void DrawViewportPanel();
        void DrawContentPanel();
        void FindRendererLayer();
        
        // 事件处理函数
        bool OnFileDragDrop(FileDragDropEvent& event);

    private:
        RendererLayer* m_RendererLayer = nullptr;
        Ref<Scene> m_Scene; // 当前场景
        glm::vec2 m_ViewportSize{0.0f, 0.0f};
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
    };
}