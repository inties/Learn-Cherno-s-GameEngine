#include "pch.h"

#include "TextureLibrary.h"

namespace Engine {

Ref<TextureLibrary> TextureLibrary::Get() {

    static Ref<TextureLibrary> instance = CreateRef<TextureLibrary>();

    return instance;

}

Ref<Texture2D> TextureLibrary::Load(const std::string& path) {

    if (Exists(path)) return Get(path);

    Ref<Texture2D> texture = Texture2D::Create(path);

    Add(path, texture);

    return texture;

}

Ref<Texture2D> TextureLibrary::Get(const std::string& key) {

    if (m_Textures.find(key) != m_Textures.end()) return m_Textures[key];

    return nullptr;

}

bool TextureLibrary::Exists(const std::string& key) const {

    return m_Textures.find(key) != m_Textures.end();

}

void TextureLibrary::Add(const std::string& key, const Ref<Texture2D>& texture) {

    m_Textures[key] = texture;

}

} 