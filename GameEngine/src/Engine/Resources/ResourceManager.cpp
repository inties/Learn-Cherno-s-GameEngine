#include "pch.h"
#include "Engine/Resources/ResourceManager.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Model/Model.h"

namespace Engine {

    Ref<ResourceManager> ResourceManager::Get() {
        static Ref<ResourceManager> s_Instance = CreateRef<ResourceManager>();
        return s_Instance;
    }

    Ref<Model> ResourceManager::GetModel(const std::string& relativePath) const {
        auto it = m_ModelCache.find(relativePath);
        if (it != m_ModelCache.end()) return it->second;
        return nullptr;
    }

    Ref<Model> ResourceManager::LoadModel(const std::string& relativePath) {
        // 统一 key
        std::string key = ProjectManager::NormalizePath(relativePath);
        auto it = m_ModelCache.find(key);
        if (it != m_ModelCache.end()) return it->second;

        // 拼接绝对路径
        auto pm = ProjectManager::Get();
        const std::string& root = pm->GetProjectRoot();
        if (root.empty()) return nullptr;
        std::string abs = root;
        if (!abs.empty() && abs.back() != '/') abs += '/';
        abs += key;

        Ref<Model> model = Model::Create(abs);
        if (model) {
            m_ModelCache[key] = model;
        }
        return model;
    }

    std::vector<std::string> ResourceManager::ListModelKeys() const {
        std::vector<std::string> keys;
        keys.reserve(m_ModelCache.size());
        for (auto& kv : m_ModelCache) keys.push_back(kv.first);
        return keys;
    }
}