#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Engine/core.h"
#include "Engine/Model/Model.h"

namespace Engine {

    // ��С��Դ�����������水���·�������� Model
    class ResourceManager {
    public:
        static Ref<ResourceManager> Get();

        // ��ȡ�����ģ�ͣ�·������Ϊ��� ProjectRoot ��ͳһ��б�ܷ��
        Ref<Model> LoadModel(const std::string& relativePath);
        Ref<Model> GetModel(const std::string& relativePath) const;

        // ����ͳ��/����
        std::vector<std::string> ListModelKeys() const;

    private:
        std::unordered_map<std::string, Ref<Model>> m_ModelCache;
    };
}