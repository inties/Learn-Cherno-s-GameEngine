#pragma once
#include"pch.h"
#include"Layer.h"
#include"Event.h"
namespace Engine {

	class ENGINE_API LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);
		void IterateLayers();
		bool ProcessEvents(Event& e);
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}