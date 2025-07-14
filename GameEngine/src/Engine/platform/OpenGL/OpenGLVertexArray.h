#pragma once

#include "Engine/Renderer/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const override { return m_VertexBuffer; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

		// Helper methods for rendering logic
		virtual bool HasIndexBuffer() const override;
		virtual uint32_t GetVertexCount() const override;

	private:
		uint32_t m_RendererID;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
} 