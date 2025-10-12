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
		std::unordered_map<std::string, Ref<ResourceType>> Registry;
		Ref<ResourceType> Get(const std::string& name) {//资源不再被任何对象引用时（计数为1？），应当主动销毁。
			auto it = Registry.find(name);
			if (it != Registry.end()) {
				return it->second;
			}
			ENGINE_CORE_INFO("{} unloaded resource", name);
			return nullptr;
		}
		template<typename...Args>
		void Create(const std::string& name, Args&&...args) {
			Ref<ResourceType> resource = CreateRef<ResourceType>(std::forward<Args>(args)...);
			if (!resource) {
				ENGINE_CORE_ERROR("{}failed to create resource", name);
				return;
			}
			Registry[name] = resource;
		}
		void Regist(const std::string& name, const Ref<ResourceType>&res) {
			if (Get(name)) {
				ENGINE_INFO("{}twice registered", name);
				return;
			}
			else if (!res) {
				ENGINE_INFO("{}registing a nullptr", name);
				return;
			}
			Registry[name] = res;
		}
		void Remove(const std::string& name) {
			auto it = Registry.find(name);
			if (it == Registry.end()) return;
			auto res = it->second;
			if (res.use_count() > 2) { // Registry中的引用 + 当前局部变量
				ENGINE_CORE_INFO("{}can't destroy", name);
				return;
			}
			Registry.erase(it); // 完全移除条目
		}
	
	};
}
