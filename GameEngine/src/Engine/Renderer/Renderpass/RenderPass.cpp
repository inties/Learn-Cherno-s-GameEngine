#include "pch.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
void Engine::Pre_Z_Pass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
{
			FBO->ColorMask(false);//��ֹ��ɫд��
			FBO->Bind();
			m_shader->Bind();
			MainCamera* camera = MainCamera::GetInstance();
			if (!camera) {
				ENGINE_CORE_ERROR("No camera available for instanced rendering");
				return;
			}
			glm::mat4 viewMatrix = camera->GetViewMatrix();
			glm::mat4 projMatrix = camera->GetProjectionMatrix();
			glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
			// ��Ⱦÿ������
			for (auto& [key, batchData] : *batch_data) {
				if (batchData.instances.empty()) continue;
				// ������ͼͶӰ����
				m_shader->SetMat4("u_ViewProjection", viewProjMatrix);
				batchData.ssbo->Bind(0);
				// ��VAO
				key.vao->Bind();
				// ִ��ʵ��������
				uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
				RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
			}
			FBO->ColorMask(true);

			tiled_z->Clear();
			RenderCommand::InsertBarrier(BarrierDomain::RenderTargetWriteToSample);
			auto depthtexture=FBO->GetDepth();
			auto tile_depth_shader = m_pipeline_settings.ShaderManager->Get("tile_depth").get();
			tile_depth_shader->Bind();
			depthtexture->Bind(0);
			ImageBindDesc desc;
			desc.binding = 1;
			desc.access = TextureAccess::WriteOnly;
			tiled_z->BindAsImage(desc);
	
			tile_depth_shader->SetMat4("projection", projMatrix);
			RenderCommand::Dispatch(std::ceil(tiled_z->GetWidth() / 16.0f), std::ceil(tiled_z->GetHeight() / 16.0f), 1);
			
			
}

void Engine::Pre_Z_Pass::Resize(uint32_t width,uint32_t height)
{
	tiled_z = Texture2D::Create(width, height, TextureFormat::RED32F, 1);
}
