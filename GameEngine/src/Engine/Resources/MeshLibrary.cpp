#include "pch.h"

#include "MeshLibrary.h"

namespace Engine {

Ref<MeshLibrary> MeshLibrary::Get() {

    static Ref<MeshLibrary> instance = CreateRef<MeshLibrary>();

    return instance;

}

Ref<Mesh> MeshLibrary::Get(const std::string& key) {

    if (m_Meshes.find(key) != m_Meshes.end()) return m_Meshes[key];

    return nullptr;

}

bool MeshLibrary::Exists(const std::string& key) const {

    return m_Meshes.find(key) != m_Meshes.end();

}

void MeshLibrary::Add(const std::string& key, const Ref<Mesh>& mesh) {

    m_Meshes[key] = mesh;

}

} 