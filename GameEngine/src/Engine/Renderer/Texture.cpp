//******************************************************************
//steal form hazel
#include "pch.h"
#include "Engine/Renderer/Texture.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/platform/OpenGL/OpenGLTexture.h"

namespace Engine {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height,TextureFormat format,int sample)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height,format,sample);
		}

		ENGINE_CORE_ERROR("Unknown RendererAPI!");
		
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		ENGINE_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	Scope<TextureCube> TextureCube::Create(const std::vector<std::string>& faces)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLTextureCube>(faces);
		}

		ENGINE_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	Scope<TextureCube> TextureCube::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLTextureCube>(path);
		}

		ENGINE_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

}
