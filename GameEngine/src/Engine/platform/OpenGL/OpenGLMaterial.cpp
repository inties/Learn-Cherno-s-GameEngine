#include "pch.h"

#include "OpenGLMaterial.h"

#include "Engine/Renderer/Shader.h"

namespace Engine {

OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader) : m_Shader(shader) {}

OpenGLMaterial::~OpenGLMaterial() {}

void OpenGLMaterial::Bind() const {

    m_Shader->Bind();

    for (const auto& [name,pair] : m_Textures) {

        pair.first->Bind(pair.second);

    }

    // TODO: 绑定其他uniforms

}

void OpenGLMaterial::Unbind() const {

    m_Shader->Unbind();

    // TODO: 解绑纹理如果需要

}

void OpenGLMaterial::SetInt(const std::string& name, int value) {
    m_Shader->Bind();
    m_Shader->SetInt(name, value);

}

void OpenGLMaterial::SetFloat(const std::string& name, float value) {
    m_Shader->Bind();
    m_Shader->SetFloat(name, value);

}

void OpenGLMaterial::SetFloat3(const std::string& name, const glm::vec3& value) {
    m_Shader->Bind();
    m_Shader->SetFloat3(name, value);

}

void OpenGLMaterial::SetFloat4(const std::string& name, const glm::vec4& value) {
    m_Shader->Bind();
    m_Shader->SetFloat4(name, value);

}

void OpenGLMaterial::SetMat4(const std::string& name, const glm::mat4& value) {
    m_Shader->Bind();
    m_Shader->SetMat4(name, value);

}

//name暂时没有用
void OpenGLMaterial::SetTexture(const std::string& name, const Ref<Texture>& texture,int slot) {

    m_Textures[name] = { texture,slot };

}

}
