#pragma once

#include "Engine/Renderer/Material.h"

#include <unordered_map>

#include <string>

namespace Engine {

class OpenGLMaterial : public Material {

public:

    OpenGLMaterial(const Ref<Shader>& shader);

    virtual ~OpenGLMaterial();

    virtual void Bind() const override;

    virtual void Unbind() const override;

    virtual Ref<Shader> GetShader() const override { return m_Shader; }

    virtual void SetShader(const Ref<Shader>& shader) override { m_Shader = shader; }

    virtual void SetFloat(const std::string& name, float value) override;

    virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;

    virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

    virtual void SetTexture(const std::string& name, const Ref<Texture2D>& texture) override;

private:

    Ref<Shader> m_Shader;

    std::unordered_map<std::string, Ref<Texture2D>> m_Textures;

    // TODO: 其他uniform缓存，如floats, vec3等

    mutable int m_TextureSlot = 0; // 用于绑定时分配slot

};

} 