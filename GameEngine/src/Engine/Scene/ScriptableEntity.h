#pragma once
#include "pch.h"
#include "Entity.h"


namespace Engine {
	class ScriptableEntity {
	public:
		ScriptableEntity(Entity* e) :m_Entity(e) {};
		virtual ~ScriptableEntity() = default;
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDestory();
		template<typename T>
		inline T& GetComponent() {
			return m_Entity->GetComponent<T>();
		}
	private:
		Entity* m_Entity;
	};
}