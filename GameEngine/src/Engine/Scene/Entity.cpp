#include "pch.h"
#include "Entity.h"

namespace Engine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
		AddComponent<TransformComponent>(glm::vec3(1.0f, 1.0f, 1.0f));
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