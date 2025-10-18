#pragma once
#include "pch.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/VertexArray.h"
namespace Engine {
	class ScriptableEntity;

	// Forward declare a deleter implemented in Component.cpp to avoid needing the full
	// definition of ScriptableEntity in this header.
	void DestroyScriptInstance(ScriptableEntity*& instance);
	struct TransformComponent
	{
		
	public:
		TransformComponent()=default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {
			dirty = true;
			UpdateTransform();
		}
		
		inline bool isDirty() const { return dirty; }
		inline const glm::mat4& GetTransform() const
		{
			return transform;
		}
		void Translate(glm::vec3& t) {
			Translation += t;
			dirty = true;
			UpdateTransform();
		}
	private:
		void UpdateTransform() {
			if (!dirty)return;
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			transform=glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
			dirty = false;
		}

		glm::vec3 Translation = { 0.0f,0.0f, 1.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		glm::mat4 transform = glm::mat4(1.0f);
		
		bool dirty = false;
	};

	struct TagComponent {
		std::string Name;
		TagComponent() { Name = "Undefined"; };
		TagComponent(std::string n) :Name(n) {};
	};

	struct NativeScriptableComponent {
		ScriptableEntity* Instance;
		std::function<ScriptableEntity* ()> Initilize;
		std::function<void(ScriptableEntity*&)>DestroyScript;
		template<typename T,typename...Args>
		void Bind(Args&&...args) {
			Initilize = [=]() {
				return static_cast<ScriptableEntity*>(new T(args...));
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
