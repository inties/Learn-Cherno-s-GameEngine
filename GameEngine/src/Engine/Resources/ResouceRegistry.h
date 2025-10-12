#pragma once
#include "pch.h"
namespace Engine {

	//将资源和卸载函数等元数据绑定，暂时弃用
	template<typename ResourceType>
	class ResourceEntry {
	public:
		ResourceEntry(Ref<ResourceType> res, const std::string& name) : Resource(res), Name(name) {};
		Ref<ResourceType> Resource;
		std::string Name;
		
		std::function<bool()> Destroy = []()->bool {return true;};//销毁时调用
	};
	
	template<typename ResourceType>
	class ResourceRegistry {
	public:
		std::unordered_map<std::string, Scope<ResourceType>> Registry;
		Ref<ResourceType> Get(const std::string& name) {//资源不再被任何对象引用时（计数为1？），应当主动销毁。
			auto it = Registry.find(name);
			if (it != Registry.end()) {
				return Ref<ResourceType>(it->second.get(), [](ResourceType*) {});
			}
			ENGINE_CORE_INFO("{} unloaded resource", name);
			return nullptr;
		}
		template<typename...Args>
		void Create(const std::string& name, Args&&...args) {
			Scope<ResourceType> resource = CreateScope<ResourceType>(std::forward<Args>(args)...);
			if (!resource) {
				ENGINE_CORE_ERROR("{}failed to create resource", name);
				return;
			}
			Registry[name] = std::move(resource);
		}
		void Regist(const std::string& name, Scope<ResourceType>&& res) {
			if (Get(name)) {
				ENGINE_INFO("{}twice registered", name);
				return;
			}
			else if (!res) {
				ENGINE_INFO("{}registing a nullptr", name);
				return;
			}
			//再次使用std::move，将res作为右值处理
			Registry[name] = std::move(res);
		}

		//移除后可能出现悬空指针错误
		void Remove(const std::string& name) {
			auto it = Registry.find(name);
			if (it == Registry.end()) return;
			auto res = it->second;		
			Registry.erase(it); // 完全移除条目
		}

	};
}
