#pragma once
#include"pch.h"
#include"Event.h"

namespace Engine {
	class ENGINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_LayerName(name) {}
		virtual ~Layer() = default;	
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		inline const std::string& GetName() const { return m_LayerName; }
	protected:
		std::string m_LayerName;
	};
}
