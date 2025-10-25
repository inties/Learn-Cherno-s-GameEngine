#pragma once
#include "pch.h"
#include "Entity.h"
#include "PrefabTypes.h"
namespace Engine {
	class CubeEntity :public Entity {
	public:
		CubeEntity(entt::entity handle, Scene* scene):Entity(handle,scene) {
			AddComponent<RenderComponent>("cube","cube");
		}
	};
	class SphereEntity :public Entity {
	public:
		SphereEntity(entt::entity handle, Scene* scene) :Entity(handle, scene) {
			AddComponent<RenderComponent>("sphere", "sphere");
		}
	};
	class PlaneEntity :public Entity {
	public:
		PlaneEntity(entt::entity handle, Scene* scene) :Entity(handle, scene) {
			AddComponent<RenderComponent>("plane", "sphere");
		}
	};
}
