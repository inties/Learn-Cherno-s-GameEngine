#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "Engine/camera.h"
#include "Engine/Renderer/Mesh.h"
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
	void Renderer::BeginScene() {
	}

	void Renderer::EndScene()
	{
		// TODO: Execute render commands
	}

	void Renderer::Submit(const Ref<Mesh>& mesh, const glm::mat4& transform) {
		auto material = mesh->GetMaterial();
		material->Bind();

		// ???????任????
		material->GetShader()->SetMat4("u_Model", transform);

		// ??Camera???????ViewProjection????
		MainCamera* camera = MainCamera::GetInstance();
		if (camera) {
			glm::mat4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
			material->GetShader()->SetMat4("u_ViewProjection", viewProjection);
		}
		else {
			static bool errorLogged = false;
			if (!errorLogged) {
				ENGINE_CORE_ERROR("No camera instance available for mesh rendering");
				errorLogged = true;
			}
		}

		// ?????VAO???????????
		mesh->GetSharedVAO()->Bind();
		RenderCommand::DrawIndexedWithOffset(mesh->GetSharedVAO(),
			mesh->GetIndexOffset(), mesh->GetIndexCount(), mesh->GetVertexOffset());
	}


	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		shader->Bind();

		shader->SetMat4("u_Model", transform);

		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

		vertexArray->Bind();

		if (vertexArray->GetIndexBuffer()) {

			RenderCommand::DrawIndexed(vertexArray);

		}
		else {

			RenderCommand::DrawArrays(vertexArray);

		}
	}
} // namespace Engine
