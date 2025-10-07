#pragma once
#include "pch.h"

#include "Scene.h"

#include "Component.h"
namespace Engine {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		template<typename T>
		void OnAddComponent() {
		}
		
		template<typename T>
		void OnRemoveComponent() {

		}


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ENGINE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			OnAddComponent<T>();
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			OnRemoveComponent<T>();
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		bool IsValid() const { return m_EntityHandle != entt::null && m_Scene != nullptr; }

		// Static factory method for creating Entity without adding default components
		static Entity CreateFromHandle(entt::entity handle, Scene* scene);

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};

	// Template specializations - defined inline to avoid linker errors
	template<>
	inline void Entity::OnAddComponent<RenderComponent>() {

	}

	template<>
	inline void Entity::OnRemoveComponent<RenderComponent>() {
		//可否调用rendercomponent自身的析构？
		RenderComponent& r = GetComponent<RenderComponent>();
		r.Destroy();
	}

}
