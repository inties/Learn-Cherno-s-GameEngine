#include "pch.h"

#include "Material.h"

#include "Engine/platform/OpenGL/OpenGLMaterial.h"

namespace Engine {

Ref<Material> Material::Create(const Ref<Shader>& shader) {

    // TODO: 根据RendererAPI选择实现

    return CreateRef<OpenGLMaterial>(shader);

}

} 