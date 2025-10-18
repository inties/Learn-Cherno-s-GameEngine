#include "pch.h"
#include "OpenGLVertexArray.h"
#include "Engine/log.h"

#include <glad/glad.h>

namespace Engine
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		ENGINE_CORE_ERROR("Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray(TopologyType topology)
	{
		glCreateVertexArrays(1, &m_RendererID);
		m_topology = topology;
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		if (vertexBuffer->GetLayout().GetElements().size() == 0)
		{
			ENGINE_CORE_ERROR("Vertex Buffer has no layout!");
			return;
		}

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		uint32_t index = 0;
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				index++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribIPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset);
				index++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				uint8_t vectorSize = element.Type == ShaderDataType::Mat3 ? 3 : 4;
				for (uint8_t i = 0; i < count / vectorSize; i++)
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(index,
						vectorSize,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * vectorSize * i));
					glVertexAttribDivisor(index, 1);
					index++;
				}
				break;
			}
			default:
				ENGINE_CORE_ERROR("Unknown ShaderDataType!");
			}
		}

		m_VertexBuffer = vertexBuffer;
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	bool OpenGLVertexArray::HasIndexBuffer() const
	{
		return m_IndexBuffer != nullptr;
	}

	uint32_t OpenGLVertexArray::GetVertexCount() const
	{
		if (m_VertexBuffer == nullptr)
		{
			ENGINE_CORE_ERROR("No vertex buffer bound to vertex array!");
			return 0;
		}

		const auto& layout = m_VertexBuffer->GetLayout();
		
		if (layout.GetStride() == 0)
		{
			ENGINE_CORE_ERROR("Vertex buffer has no layout or stride is zero!");
			return 0;
		}
		
		return m_VertexBuffer->GetSize() / layout.GetStride();
	}
} 
