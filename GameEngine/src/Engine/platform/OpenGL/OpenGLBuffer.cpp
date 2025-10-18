#include "pch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size)
		: m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// ShaderStorageBuffer //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t size, uint32_t binding)
		: m_Size(size), m_Binding(binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RendererID);
	}

	OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLShaderStorageBuffer::Bind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RendererID);
	}

	void OpenGLShaderStorageBuffer::Unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void OpenGLShaderStorageBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
	}

	void OpenGLShaderStorageBuffer::EnsureCapacity(uint32_t requiredSize)
	{
		if (requiredSize > m_Size) {
			// 重新创建更大的缓冲区
			glDeleteBuffers(1, &m_RendererID);
			m_Size = requiredSize;
			glCreateBuffers(1, &m_RendererID);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
			glBufferData(GL_SHADER_STORAGE_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RendererID);
		}
	}
} 
