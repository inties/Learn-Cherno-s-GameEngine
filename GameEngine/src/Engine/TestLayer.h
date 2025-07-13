#pragma once

#include "Engine.h"
#include "platform/OpenGL/OpenGLShader.h"
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
	std::string GetShaderPath(const std::string& filename);

	std::shared_ptr<Engine::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Engine::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<Engine::VertexArray> m_VertexArray;
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::Shader> m_BlueShader;
}; 