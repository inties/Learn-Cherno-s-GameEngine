#pragma once
#include "pch.h"
#include "Entity.h"
#include "Component.h"

namespace Engine {
	class ScriptableEntity {
	public:
		ScriptableEntity(Entity entity) : m_Entity(entity) {}
		virtual ~ScriptableEntity() = default;
		

		virtual void OnCreate() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}
		

		template<typename T>
		inline T& GetComponent() {
			return m_Entity.GetComponent<T>();
		}
	protected:
		Entity m_Entity;
	};
	
	class MoveScript :public ScriptableEntity {
	public:
		MoveScript(Entity e) :ScriptableEntity(e) {		
		}
		
		void OnCreate() override {	
		}
		
		void OnUpdate() override {
			auto& transform = GetComponent<TransformComponent>();
			transform.Translate(glm::vec3(0.01f, 0.0f, 0.0f));
		}
		
	};
}