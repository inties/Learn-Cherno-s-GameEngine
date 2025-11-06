#include "pch.h"
#include "RenderPass.h"
#include "RenderPipeline.h"
void Engine::Pre_Z_Pass::Draw(std::unordered_map<BatchKey, BatchData, BatchKeyHash>* batch_data)
{
			FBO->ColorMask(false);//禁止颜色写入
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

			// 渲染非透明pass
			for (auto& [key, batchData] : batch_data[(int)RenderItemLayer::Opaque]) {
				if (batchData.instances.empty()) continue;
				// 设置视图投影矩阵
				m_shader->SetMat4("u_ViewProjection", viewProjMatrix);

				batchData.ssbo->Bind(0);
				// 绑定VAO
				key.vao->Bind();
				// 执行实例化绘制
				uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
				RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
			}
			
		
			FBO->ColorMask(true);

			//光源剔除，生成每个tile的可见光源列表
			CullLights();
			
			
			
}

void Engine::Pre_Z_Pass::Resize(uint32_t width,uint32_t height)
{
	debug_texture = Texture2D::Create(width, height, TextureFormat::RGBA16,1);
	visible_lights = ShaderStorageBuffer::Create(std::ceil(width/16.0f)* std::ceil(height / 16.0f)*1024*sizeof(int));
}

void Engine::Pre_Z_Pass::CullLights()
{
	const int minus1 = -1;
	glClearNamedBufferData(visible_lights->GetRenderID(), GL_R32I, GL_RED_INTEGER, GL_INT, &minus1);
	MainCamera* camera = MainCamera::GetInstance();
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glm::mat4 projMatrix = camera->GetProjectionMatrix();
	glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
	debug_texture->Clear();
	RenderCommand::InsertBarrier(BarrierDomain::RenderTargetWriteToSample);
	auto depthtexture = FBO->GetDepth();
	auto tile_depth_shader = m_pipeline_settings.ShaderManager->Get("culling_lights").get();
	tile_depth_shader->Bind();
	depthtexture->Bind(0);
	ImageBindDesc desc;
	desc.binding = 1;
	desc.access = TextureAccess::WriteOnly;
	debug_texture->BindAsImage(desc);


	m_pipeline_settings.lights_gpu->Bind(0);
	visible_lights->Bind(1);

	tile_depth_shader->SetFloat("max_visible_distance", 100.f);
	tile_depth_shader->SetMat4("view", viewMatrix);
	tile_depth_shader->SetMat4("projection", projMatrix);
	glm::mat4 inv_VP = glm::inverse(viewProjMatrix);
	tile_depth_shader->SetMat4("viewProjection", viewProjMatrix);
	tile_depth_shader->SetMat4("inv_VP_matrix", inv_VP);
	tile_depth_shader->SetInt("points_light_count", 1000);
	tile_depth_shader->SetInt("spot_light_count", 100);


	RenderCommand::Dispatch(std::ceil(debug_texture->GetWidth() / 16.0f), std::ceil(debug_texture->GetHeight() / 16.0f), 1);
}
