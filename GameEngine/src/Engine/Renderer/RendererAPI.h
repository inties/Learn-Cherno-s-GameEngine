#pragma once

#include "pch.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

namespace Engine
{
	enum class BarrierDomain
	{
		ComputeWriteToComputeRead,   // compute writes SSBO/image, next compute reads it
		ComputeWriteToGraphicsRead,  // compute writes texture/image, next draw samples it
		RenderTargetWriteToSample,   // FBO color attachment now used as sampler
		ComputeWriteToRenderTarget	 // compute writes image to FBO
	};
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
		

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void SetWildFrame(bool enable) = 0;
		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;
		virtual void InsertBarrier(const BarrierDomain& barrier)const = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount = 0) = 0;
		virtual void DrawIndexedWithOffset(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset) = 0;
		
		// 实例化绘制
		virtual void DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexCount, uint32_t instanceCount) = 0;
		virtual void DrawIndexedInstanced(const VertexArray* vertexArray, 
			uint32_t indexCount, uint32_t instanceCount) = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RendererAPI> Create();
	private:
		static API s_API;
	};
} 
