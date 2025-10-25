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
			// 渲染每个批次
			for (auto& [key, batchData] : *batch_data) {
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
}
