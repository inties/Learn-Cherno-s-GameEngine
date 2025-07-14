#include "pch.h"
#include "Renderer.h"
#include "Shader.h"

namespace Engine
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::Shutdown()
	{
		// TODO: Cleanup resources
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene()
	{
		// TODO: Set up scene parameters (camera, lights, etc.)
	}

	void Renderer::EndScene()
	{
		// TODO: Execute render commands
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		// TODO: Set up shader uniforms with transform and scene data
		shader->Bind();
		vertexArray->Bind();
		
		// Use helper method to determine drawing mode
		if (vertexArray->HasIndexBuffer())
		{
			// Use indexed drawing when index buffer is present
			RenderCommand::DrawIndexed(vertexArray);
		}
		else
		{
			// Use array drawing when no index buffer is present
			RenderCommand::DrawArrays(vertexArray);
		}
	}
} 