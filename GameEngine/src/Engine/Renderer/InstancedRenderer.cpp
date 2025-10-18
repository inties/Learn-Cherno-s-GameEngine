#include "pch.h"
#include "InstancedRenderer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/camera.h"

namespace Engine
{
	InstancedRenderer::InstancedRenderer()
	{
	}

	InstancedRenderer::~InstancedRenderer()
	{
		Clear();
	}

	void InstancedRenderer::Init()
	{
		// 初始化完成
		ENGINE_CORE_INFO("InstancedRenderer initialized");
	}

	void InstancedRenderer::CollectInstances(Scene* scene, ResourceRegistry<VertexArray>* vaoManager, ResourceRegistry<Material>* matManager)
	{
		if (!scene || !vaoManager || !matManager) {
			ENGINE_CORE_ERROR("InstancedRenderer: Scene/VAOmanager/MatManager为空");
			return;
		}

		// 清空之前的批次
		 //Clear();

		// 获取场景的registry
		auto& registry = scene->GetRegistry();

		// 使用entt的view方法遍历有RenderComponent和TransformComponent的实体
		static int i = 0;
		auto group = registry.group<RenderComponent>(entt::get<TransformComponent>);
		if (i == 0) {
			group.each([&](auto entity, auto& renderComp, auto& transComp) {
				// 绑定资源（如果还未绑定）
				if (!renderComp.IsValid()) {
					renderComp.BindResources(*vaoManager, *matManager);


					// 如果资源绑定成功，添加到批次
					if (renderComp.IsValid()) {
						BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
					}



					// 创建实例数据
					InstanceData instanceData;


					instanceData.modelMatrix = transComp.GetTransform();
					instanceData.extraData = glm::vec4(static_cast<float>(static_cast<int>(entity)), 0.0f, 0.0f, 0.0f);

					// 添加到对应批次

					BatchKey key{ renderComp.VAO.get(), renderComp.Mat.get() };
					m_Batches[key].instances.push_back(instanceData);

				}

				});
		}

		/*group.each([&](auto entity, auto& renderComp, auto& transComp) {
			});*/

		i++;
		//auto view = registry.view<RenderComponent, TransformComponent>();
	/*	for (auto& [entity, renderComp, transComp] : view.each()) {
			
		}*/
		// 为每个批次创建SSBO
		for (auto& [key, batchData] : m_Batches) {
			uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
			
			// 确保不超过最大实例数
			if (instanceCount > MAX_INSTANCES_PER_BATCH) {
				ENGINE_CORE_WARN("Instance count {} exceeds maximum {}, truncating", instanceCount, MAX_INSTANCES_PER_BATCH);
				instanceCount = MAX_INSTANCES_PER_BATCH;
				batchData.instances.resize(instanceCount);
			}
			
			// 使用固定大小SSBO（与着色器中的数组大小匹配）
			uint32_t ssboSize = MAX_INSTANCES_PER_BATCH * sizeof(InstanceData);
			
			// 确保SSBO容量足够
			if (!batchData.ssbo) {
				batchData.ssbo = ShaderStorageBuffer::Create(ssboSize, INSTANCE_SSBO_BINDING);
				// 上传实例数据到SSBO			
			}

			// 上传实例数据到SSBO
			uint32_t dataSize = instanceCount * sizeof(InstanceData);
			batchData.ssbo->SetData(batchData.instances.data(), dataSize);
			batchData.maxInstances = instanceCount;
		}

		// ENGINE_CORE_INFO("Collected {} batches with total instances", m_Batches.size());
	}

	void InstancedRenderer::RenderInstanced()
	{
		// 获取相机矩阵
		Camera* camera = Camera::GetInstance();
		if (!camera) {
			ENGINE_CORE_ERROR("No camera available for instanced rendering");
			return;
		}

		glm::mat4 viewMatrix = camera->GetViewMatrix();
		glm::mat4 projMatrix = camera->GetProjectionMatrix();
		glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

		// 渲染每个批次
		for (auto& [key, batchData] : m_Batches) {
			if (batchData.instances.empty()) continue;

			// 绑定材质和着色器
			key.material->Bind();
			auto shader = key.material->GetShader();
			shader->Bind();
			
			// 设置视图投影矩阵
			shader->SetMat4("u_ViewProjection", viewProjMatrix);
			shader->SetFloat3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
			shader->SetFloat3("cameraPos_ws", camera->GetPosition());
			// 绑定SSBO
			batchData.ssbo->Bind();
			
			// 绑定VAO
			key.vao->Bind();
			
			// 执行实例化绘制
			uint32_t instanceCount = static_cast<uint32_t>(batchData.instances.size());
			RenderCommand::DrawIndexedInstanced(key.vao, 0, instanceCount);
		}
	}

	void InstancedRenderer::Clear()
	{
		m_Batches.clear();
	}
}
