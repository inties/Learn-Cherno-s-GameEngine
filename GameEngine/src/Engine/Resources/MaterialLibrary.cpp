#include "pch.h"

#include "MaterialLibrary.h"

namespace Engine {

Ref<MaterialLibrary> MaterialLibrary::Get() {

    static Ref<MaterialLibrary> instance = CreateRef<MaterialLibrary>();

    return instance;

}

Ref<Material> MaterialLibrary::Get(const std::string& key) {

    if (m_Materials.find(key) != m_Materials.end()) return m_Materials[key];

    return nullptr;

}

bool MaterialLibrary::Exists(const std::string& key) const {

    return m_Materials.find(key) != m_Materials.end();

}

void MaterialLibrary::Add(const std::string& key, const Ref<Material>& material) {

    m_Materials[key] = material;

}

} 