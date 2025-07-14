# Renderer & ImGui Integration – Design Recommendations

## 1. Architectural Overview

``Layer`` (see `Engine/Layer.h`) already provides a modular way to plug subsystems into the core `Application` loop (`OnAttach`, `OnDetach`, `OnUpdate`, `OnImGuiRender`, `OnEvent`).  Modern engines usually dedicate **two specialised layers** for rendering and UI:

1. **RendererLayer** – owns all GPU resources & draw-calls.
2. **ImGuiLayer**   – initialises Dear ImGui context, converts engine events → ImGui IO, and **renders last**.

> The RendererLayer should render *scene* objects; the ImGuiLayer should draw *editor/overlay* widgets.

## 2. Relationship between the Layers

```
Application
 ├─ RendererLayer   (update / submit / present)
 └─ ImGuiLayer      (begin frame → draw widgets → end frame)
```

* RendererLayer **does not depend** on ImGuiLayer.
* ImGuiLayer **pulls data** from RendererLayer via exposed handles (pointers/refs, singletons or ECS queries).
* Ordering in `Application::Run()`:
  1. Iterate layers → `OnUpdate()` (RendererLayer fills command-buffers)
  2. Begin ImGui frame (ImGuiLayer::Begin)
  3. Iterate layers → `OnImGuiRender()` (RendererLayer publishes controls, ImGuiLayer shows demo etc.)
  4. ImGuiLayer::End → render draw-data using RendererAPI

## 3. Exposing Renderer Variables to ImGui

1. **Public Config Struct**
   ```cpp
   struct RendererSettings {
       glm::vec3 clearColor{0.1f,0.1f,0.1f};
       bool vsync = true;
       bool wireframe = false;
   };
   ```
   Stored inside `RendererLayer`, returned by `GetSettings()` (reference).

2. **Callbacks / Observers** – ImGuiLayer edits settings; RendererLayer watches or is informed via setter.

3. **ImGuiRender Hook** – simplest: RendererLayer implements `OnImGuiRender()` and draws its own panel:
   ```cpp
   void RendererLayer::OnImGuiRender()
   {
       if(ImGui::Begin("Renderer Settings"))
       {
           ImGui::ColorEdit3("Clear Color", &m_Settings.clearColor.x);
           ImGui::Checkbox("Wireframe", &m_Settings.wireframe);
       }
       ImGui::End();
   }
   ```

ImGuiLayer just provides Begin/End; *any* layer can push its own widget tree.

## 4. Event System Integration

ImGui needs raw input.  Current ImGuiLayer already intercepts events and sets `m_BlockEvents`:

* If ImGui wants the event → mark `e.m_handled = true`.
* Else → allow propagation so other layers (e.g., game logic) still receive it.

RendererLayer rarely consumes input directly; if required (e.g., camera controller) it can listen **after** ImGuiLayer so unhandled events reach it.

## 5. `UpdateImGui()` in RendererLayer?

Instead of a separate `UpdateImGui()`:

* Use the existing `OnImGuiRender()` callback.
* Keep rendering logic in `OnUpdate()` / `Submit()`.
* Expose run-time stats (FPS, draw-calls, GPU timers) inside `OnImGuiRender()`.

## 6. Minimal Implementation Steps

1. Create `RendererLayer : public Layer` – move `Renderer::BeginScene/EndScene/Submit` orchestration here.
2. Ensure `Application` pushes layers in order: `RendererLayer`, then `ImGuiLayer`.
3. Expose `RendererSettings` & stats via getters or public reference.
4. Implement `RendererLayer::OnImGuiRender()` to edit settings.
5. In `RendererLayer::OnUpdate()` apply settings before calling `RenderCommand` (e.g., clear-color, wireframe mode).
6. Confirm ImGuiLayer intercepts input and sets `m_BlockEvents` correctly.

---
**Outcome:**
• Loose coupling; Renderer can run headless without ImGui.  
• ImGui widgets can tweak renderer state in real-time.  
• No changes needed to core event system beyond existing ImGuiLayer interception. 