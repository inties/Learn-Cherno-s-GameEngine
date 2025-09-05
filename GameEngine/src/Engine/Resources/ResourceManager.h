#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Engine/core.h"
#include "Engine/Model/Model.h"

namespace Engine {

    // 最小资源管理器：缓存按相对路径创建的 Model
    class ResourceManager {
    public:
        static Ref<ResourceManager> Get();

        // 获取或加载模型（路径必须为相对 ProjectRoot 的统一正斜杠风格）
        Ref<Model> LoadModel(const std::string& relativePath);
        Ref<Model> GetModel(const std::string& relativePath) const;

        // 简易统计/遍历
        std::vector<std::string> ListModelKeys() const;

    private:
        std::unordered_map<std::string, Ref<Model>> m_ModelCache;
    };
}