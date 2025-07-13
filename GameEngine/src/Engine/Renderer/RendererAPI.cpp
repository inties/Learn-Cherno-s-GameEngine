#include "pch.h"
#include "RendererAPI.h"
#include "Engine/log.h"

#include "Engine/platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	std::unique_ptr<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:
			ENGINE_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_unique<OpenGLRendererAPI>();
		}

		ENGINE_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}
} 