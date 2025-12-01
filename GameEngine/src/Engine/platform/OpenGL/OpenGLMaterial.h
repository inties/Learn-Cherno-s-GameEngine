#pragma once

#include "Engine/Renderer/Material.h"

#include <unordered_map>

#include <string>

namespace Engine {

class OpenGLMaterial : public Material {

public:

    OpenGLMaterial(const Ref<Shader>& shader);

    virtual ~OpenGLMaterial();

    void Bind() const override;

    void Unbind() const override;

    Ref<Shader> GetShader() const override { return m_Shader; }

    void SetShader(const Ref<Shader>& shader) override { m_Shader = shader; }

    void SetInt(const std::string& name, int value) override;

    void SetFloat(const std::string& name, float value) override;

    void SetFloat3(const std::string& name, const glm::vec3& value) override;

    void SetFloat4(const std::string& name, const glm::vec4& value) override;

    void SetMat4(const std::string& name, const glm::mat4& value) override;

    void SetTexture(const std::string& name, const Ref<Texture>& texture,int slot) override;

private:

    

    std::unordered_map<std::string, std::pair<Ref<Texture>,int>> m_Textures;


    Ref<Shader> m_Shader;
};

}
