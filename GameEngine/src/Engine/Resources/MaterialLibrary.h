#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Material.h"

#include <unordered_map>

#include <string>

namespace Engine {

class MaterialLibrary {

public:

    static Ref<MaterialLibrary> Get();

    Ref<Material> Get(const std::string& key);

    bool Exists(const std::string& key) const;

    void Add(const std::string& key, const Ref<Material>& material);

public:
    MaterialLibrary() = default;

private:
    std::unordered_map<std::string, Ref<Material>> m_Materials;

};

} 