#include "pch.h"
#include "Engine/Renderer/FrameBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/platform/OpenGL/OpenGLFrameBuffer.h"

namespace Engine {

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
	auto renderAPI = Renderer::GetAPI();
	switch (renderAPI)
	{
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLFramebuffer>(spec);
	case RendererAPI::API::None:
		ENGINE_CORE_ASSERT(false, "RenderAPI not set!");
		return nullptr;
	default:
		break;
	}
	return Engine::Ref<Framebuffer>();
}
}
