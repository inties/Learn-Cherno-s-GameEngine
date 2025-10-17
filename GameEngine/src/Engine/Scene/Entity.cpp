#include "pch.h"
#include "Entity.h"

namespace Engine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
		// 随机生成TransformComponent初始位置
		{
			float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
			float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
			float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
			AddComponent<TransformComponent>(glm::vec3(x, y, z));
		}
		AddComponent<TagComponent>();
	}

	Entity Entity::CreateFromHandle(entt::entity handle, Scene* scene)
	{
		Entity entity;
		entity.m_EntityHandle = handle;
		entity.m_Scene = scene;
		return entity;
	}



}