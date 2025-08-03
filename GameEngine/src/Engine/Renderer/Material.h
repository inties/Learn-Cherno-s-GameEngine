#pragma once

#include "Engine/core.h"

#include "Engine/Renderer/Shader.h"

#include "Engine/Renderer/Texture.h"

#include <string>

#include <unordered_map>

#include <glm/glm.hpp>

namespace Engine {

class Material {

public:

    static Ref<Material> Create(const Ref<Shader>& shader);

    virtual ~Material() = default;

    virtual void Bind() const = 0;

    virtual void Unbind() const = 0;

    virtual Ref<Shader> GetShader() const = 0;

    virtual void SetShader(const Ref<Shader>& shader) = 0;

    // 统一设置接口

    virtual void SetFloat(const std::string& name, float value) = 0;

    virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;

    virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    // 纹理设置

    virtual void SetTexture(const std::string& name, const Ref<Texture2D>& texture) = 0;

};

} 