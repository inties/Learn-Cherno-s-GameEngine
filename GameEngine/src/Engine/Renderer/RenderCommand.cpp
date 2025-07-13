#include "pch.h"
#include "RenderCommand.h"

namespace Engine
{
	std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
} 