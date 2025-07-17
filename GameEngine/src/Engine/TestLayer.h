#pragma once

#include "Engine.h"

class TestLayer : public Engine::Layer
{
public:
	TestLayer();
	virtual ~TestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(Engine::Event& e) override;

private:
	float m_Time = 0.0f;
	int m_Counter = 0;
}; 