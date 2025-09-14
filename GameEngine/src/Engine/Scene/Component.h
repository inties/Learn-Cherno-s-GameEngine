#pragma once
#include "pch.h"
#include <glm/gtx/quaternion.hpp>
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/VertexArray.h"
namespace Engine {
	class ScriptableEntity;

	// Forward declare a deleter implemented in Component.cpp to avoid needing the full
	// definition of ScriptableEntity in this header.
	void DestroyScriptInstance(ScriptableEntity*& instance);
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {
		}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct TagComponent {
		std::string Name;
		TagComponent() { Name = "Undefined"; };
		TagComponent(std::string n) :Name(n) {};
	};

	struct NativeScriptableComponent {
		ScriptableEntity* Instance;
		ScriptableEntity* (*Initilize)();        
		void(*DestroyScript)(ScriptableEntity*&);
		template<typename T>
		void Bind() {
			Initilize = []() {
				return static_cast<ScriptableEntity*>(new T());
				};
			DestroyScript = &DestroyScriptInstance;
		}
	};

	struct RenderComponent {
		Ref<VertexArray>VAO=nullptr;
		Ref<Material>Mat=nullptr;
		std::string GeoMetryName="";
		std::string MatName="";
		std::function<void()>Destroy;
		bool geometryCreated = false;
		bool materialCreated = false;
		bool geometryFailed = false;
		bool materialFailed = false;
		RenderComponent(const std::string GeoName="mesh", const std::string MName = "defaultShader") :GeoMetryName(GeoName), MatName(MName) {};

		// Resource cleanup callbacks
		std::function<void()> geometryCleanup;
		std::function<void()> materialCleanup;

		// Helper methods
		bool IsValid() const { return geometryCreated && materialCreated && VAO && Mat; }
		bool NeedsGeometryCreation() const { return !geometryCreated && !geometryFailed && !GeoMetryName.empty(); }
		bool NeedsMaterialCreation() const { return !materialCreated && !materialFailed && !MatName.empty(); }
		bool IsFullyCreated() const { return geometryCreated && materialCreated; }

		// 从ResourceRegistry绑定资源
		template<typename VAORegistry, typename MatRegistry>
		void BindResources(VAORegistry& vaoRegistry, MatRegistry& matRegistry) {
			if (!geometryCreated && !GeoMetryName.empty()) {
				VAO = vaoRegistry.Get(GeoMetryName);
				if (VAO) {
					geometryCreated = true;
					geometryFailed = false;
				} else {
					geometryFailed = true;
				}
			}
			if (!materialCreated && !MatName.empty()) {
				Mat = matRegistry.Get(MatName);
				if (Mat) {
					materialCreated = true;
					materialFailed = false;
				} else {
					materialFailed = true;
				}
			}
		}
	};

}