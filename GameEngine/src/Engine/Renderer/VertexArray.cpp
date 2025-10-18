#include "pch.h"
#include "VertexArray.h"

#include "Engine/platform/OpenGL/OpenGLVertexArray.h"
#include "Engine/Renderer/RendererAPI.h"
#include"Renderer.h"
namespace Engine
{
	Scope<VertexArray> VertexArray::Create(TopologyType topology)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateScope<OpenGLVertexArray>(topology);
			
		}
	}
} 
