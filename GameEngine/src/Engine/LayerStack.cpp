#include"pch.h"
#include"LayerStack.h"
using namespace Engine;
LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers)
	{
		delete layer;
	}
}
void LayerStack::PushLayer(Layer* layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	layer->OnAttach();//调用层初始化方法
	m_LayerInsertIndex++;
}
void LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
	overlay->OnAttach();
}
void LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		(*it)->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}
void LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		(*it)->OnDetach();
		m_Layers.erase(it);
	}
}
void LayerStack::IterateLayers()
{
	for (Layer* layer : m_Layers)
	{
		layer->OnUpdate();
	}
}
bool LayerStack::ProcessEvents(Event& e) {
	for (Layer* layer : m_Layers)
	{
		layer->OnEvent(e);
	}
	return true;
}