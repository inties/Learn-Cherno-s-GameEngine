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
        void Init(RendererLayer* rendererlayer);
        void OnAttach() override;
        void OnDetach() override {};
        void OnUpdate() override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;
        Scene* GetScene() { return m_Scene.get(); };
    private:
        void DrawHierarchyPanel();
        void DrawInspectorPanel();
        void DrawConsolePanel();
        void DrawViewportPanel();
        void DrawContentPanel();
        void SetRendererLayer(RendererLayer* rendererlayer);
        

        //bool OnFileDragDrop(FileDragDropEvent& event);
        bool OnMouseMove(MouseMoveEvent& event);


    private:
        RendererLayer* m_RendererLayer = nullptr;
        Scope<Scene> m_Scene; 
        glm::vec2 m_ViewportSize{0.0f, 0.0f};
        glm::vec2 m_mousePos{ 0.0f,0.0f };
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool running = false;
    };
}
