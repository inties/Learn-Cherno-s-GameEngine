#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Mesh.h"

#include <unordered_map>

#include <string>

namespace Engine {

class MeshLibrary {

public:

    static Ref<MeshLibrary> Get();

    Ref<Mesh> Get(const std::string& key);

    bool Exists(const std::string& key) const;

    void Add(const std::string& key, const Ref<Mesh>& mesh);

    MeshLibrary() = default;

private:

    std::unordered_map<std::string, Ref<Mesh>> m_Meshes;

};

} 