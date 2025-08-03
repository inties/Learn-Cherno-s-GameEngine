#pragma once

#include "RenderCommand.h"
#include "VertexArray.h"
#include <glm/glm.hpp>
#include "Engine/camera.h"
#include "Engine/Renderer/Mesh.h"
namespace Engine
{
	class Shader;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(); // Ìí¼Ó
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Mesh>& mesh, const glm::mat4& transform);
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
	};
} 