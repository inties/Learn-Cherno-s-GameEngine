#pragma once

#include "pch.h"
#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		virtual const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

		// Helper methods for rendering logic
		virtual bool HasIndexBuffer() const = 0;
		virtual uint32_t GetVertexCount() const = 0;  // Calculate total vertex count

		static Scope<VertexArray> Create();
	};
} 
