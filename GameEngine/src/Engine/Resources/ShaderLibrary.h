#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Shader.h"

#include <unordered_map>

#include <string>

namespace Engine {

class ShaderLibrary {

public:

    static Ref<ShaderLibrary> Get();

    void Add(const std::string& name, const Ref<Shader>& shader);

    Ref<Shader> Load(const std::string& name, const std::string& filepath);

    Ref<Shader> Load(const std::string& name, const std::string& vertexfilepath,const std::string& fragfilepath);



    Ref<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;



    ShaderLibrary() = default;
    private:

    std::unordered_map<std::string, Ref<Shader>> m_Shaders;

};

} 
