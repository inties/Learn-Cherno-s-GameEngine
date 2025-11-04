#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t);
		OpenGLVertexBuffer(void*, uint32_t);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		
		virtual uint32_t GetSize() const override { return m_Size; }
		
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t size);
		virtual ~OpenGLShaderStorageBuffer();

		virtual void Bind(uint8_t slot) const override;
		virtual void Unbind() const override;
		uint32_t GetRenderID() const override { return m_RendererID; }
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		
		virtual uint32_t GetSize() const override { return m_Size; }

		
		virtual void EnsureCapacity(uint32_t requiredSize) override;
		
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};
} 
