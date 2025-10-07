﻿#include "pch.h"
#include "Engine/Resources/ModelManager.h"
#include "Engine/Resources/ProjectManager.h"
#include "Engine/Model/Model.h"
#include "Engine/log.h"
#include <filesystem>

namespace Engine {

    Ref<ModelManager> ModelManager::Get() {
        static Ref<ModelManager> s_Instance = CreateRef<ModelManager>();
        return s_Instance;
    }

    Ref<Model> ModelManager::GetModel(const std::string& relativePath) const {
        std::string key = ProjectManager::NormalizePath(relativePath);
        auto it = m_ModelCache.find(key);
        if (it != m_ModelCache.end()) {
            return it->second;
        }
        return nullptr;
    }

    Ref<Model> ModelManager::LoadModel(const std::string& relativePath) {
        ENGINE_CORE_INFO("Starting to load model: {}", relativePath);
        
        // 统一 key
        std::string key = ProjectManager::NormalizePath(relativePath);
        auto it = m_ModelCache.find(key);
        if (it != m_ModelCache.end()) {
            ENGINE_CORE_INFO("Model already in cache, returning directly: {}", key);
            return it->second;
        }

        // 拼接绝对路径
        auto pm = ProjectManager::Get();
        const std::string& root = pm->GetProjectRoot();
        if (root.empty()) {
            ENGINE_CORE_ERROR("Project root directory is empty, cannot load model: {}", relativePath);
            return nullptr;
        }
        
        std::string abs = root;
        if (!abs.empty() && abs.back() != '/' && abs.back() != '\\') abs += '/';
        abs += key;
        
        // 检查文件是否存在
        if (!std::filesystem::exists(abs)) {
            ENGINE_CORE_ERROR("Model file does not exist: {}", abs);
            return nullptr;
        }
        
        ENGINE_CORE_INFO("Loading model from file: {}", abs);
        Ref<Model> model = Model::Create(abs);
        if (model) {
            m_ModelCache[key] = model;
            ENGINE_CORE_INFO("Model loaded successfully and cached: {}", key);
        } else {
            ENGINE_CORE_ERROR("Model loading failed: {}", abs);
        }
        
        return model;
    }



    std::vector<std::string> ModelManager::ListModelKeys() const {
        std::vector<std::string> keys;
        keys.reserve(m_ModelCache.size());
        for (auto& kv : m_ModelCache) keys.push_back(kv.first);
        return keys;
    }
}