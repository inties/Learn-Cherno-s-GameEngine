#include "pch.h"

#include "Material.h"

#include "Engine/platform/OpenGL/OpenGLMaterial.h"

#include"Renderer.h"
namespace Engine {

Scope<Material> Material::Create(const Ref<Shader>& shader) {

    // TODO: 根据RendererAPI选择实现
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateScope<OpenGLMaterial>(shader);
	}


}

} 
