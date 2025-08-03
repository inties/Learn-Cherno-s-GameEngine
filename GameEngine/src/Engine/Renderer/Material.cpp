#include "pch.h"

#include "Material.h"

#include "Engine/platform/OpenGL/OpenGLMaterial.h"

namespace Engine {

Ref<Material> Material::Create(const Ref<Shader>& shader) {

    // TODO: ����RendererAPIѡ��ʵ��

    return CreateRef<OpenGLMaterial>(shader);

}

} 