#include "pch.h"

#include "ShaderLibrary.h"

namespace Engine {

Ref<ShaderLibrary> ShaderLibrary::Get() {

    static Ref<ShaderLibrary> instance = CreateRef<ShaderLibrary>();

    return instance;

}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {

    m_Shaders[name] = shader;

}

Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {

    Ref<Shader> shader = Shader::Create(filepath);

    Add(name, shader);

    return shader;

}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {

    Ref<Shader> shader = Shader::Create(filepath);

    Add(filepath, shader);

    return shader;

}

Ref<Shader> ShaderLibrary::Get(const std::string& name) {

    if (m_Shaders.find(name) != m_Shaders.end()) {

        return m_Shaders[name];

    }

    return nullptr;

}

bool ShaderLibrary::Exists(const std::string& name) const {

    return m_Shaders.find(name) != m_Shaders.end();

}

} 