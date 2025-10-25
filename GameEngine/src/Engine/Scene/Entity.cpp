#include "pch.h"
#include "Entity.h"

namespace Engine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
		
		{
			AddComponent<TransformComponent>();
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
