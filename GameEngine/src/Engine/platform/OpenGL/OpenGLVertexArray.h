#pragma once

#include "Engine/Renderer/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray(TopologyType =TopologyType::TriangleList);
		virtual ~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		TopologyType GetTopologyType() const override { return m_topology; }

		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const override { return m_VertexBuffer; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

		// Helper methods for rendering logic
		bool HasIndexBuffer() const override;
		uint32_t GetVertexCount() const override;

	private:
		uint32_t m_RendererID;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		TopologyType m_topology;
	};
} 
