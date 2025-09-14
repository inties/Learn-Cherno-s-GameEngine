#include "pch.h"

#include "Material.h"

#include "Engine/platform/OpenGL/OpenGLMaterial.h"

#include"Renderer.h"
namespace Engine {

Ref<Material> Material::Create(const Ref<Shader>& shader) {

    // TODO: ����RendererAPIѡ��ʵ��
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLMaterial>(shader);
	}


}

} 