#include "pch.h"
#include "Buffer.h"

#include "Engine/platform/OpenGL/OpenGLBuffer.h"
#include "Engine/Renderer/Renderer.h"
namespace Engine
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		// TODO: 根据RendererAPI选择实现
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLVertexBuffer>(size);
		}
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		// TODO: 根据RendererAPI选择实现
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLVertexBuffer>(vertices,size);
		}
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		// TODO: 根据RendererAPI选择实现
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLIndexBuffer>(indices,count);
		}
	}

	Scope<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t size)
	{
		// TODO: 根据RendererAPI选择实现
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLShaderStorageBuffer>(size);
		}
	}
} 
