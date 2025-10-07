#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Texture.h"

#include <unordered_map>

#include <string>

namespace Engine {

class TextureLibrary {

public:

    static Ref<TextureLibrary> Get();

    Ref<Texture2D> Load(const std::string& path);

    Ref<Texture2D> Get(const std::string& key);

    bool Exists(const std::string& key) const;

    void Add(const std::string& key, const Ref<Texture2D>& texture);



    TextureLibrary() = default;
    private:
    std::unordered_map<std::string, Ref<Texture2D>> m_Textures;

};

} 