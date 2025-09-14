#pragma once
#include "pch.h"
namespace Engine {

	//����Դ��ж�غ�����Ԫ���ݰ󶨣���ʱ����
	template<typename ResourceType>
	class ResourceEntry {
	public:
		ResourceEntry(Ref<ResourceType> res, const std::string& name) : Resource(res), Name(name) {};
		Ref<ResourceType> Resource;
		std::string Name;
		
		std::function<bool()> Destroy = []()->bool {return true;};//����ʱ����
	};
	
	template<typename ResourceType>    
	class ResourceRegistry {
	public:
		std::unordered_map<std::string, Ref<ResourceType>> Registry;
		Ref<ResourceType> Get(const std::string& name) {//��Դ���ٱ��κζ�������ʱ������Ϊ1������Ӧ���������١�
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
		void Regist(const std::string& name, Ref<ResourceType>&res) {
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
			if (res.use_count() > 2) { // Registry�е����� + ��ǰ�ֲ�����
				ENGINE_CORE_INFO("{}can't destroy", name);
				return;
			}
			Registry.erase(it); // ��ȫ�Ƴ���Ŀ
		}
	
	};
}