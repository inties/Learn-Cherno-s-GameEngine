#pragma once

#include "RendererAPI.h"

namespace Engine
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}
		static void SetWildFrame(bool enable)
		{
			
			s_RendererAPI->SetWildFrame(enable);
			
		}
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		static void SetBlendState(const BlendDesc& desc) {
			s_RendererAPI->SetBlendState(desc);
		}
		static void Clear()
		{
			s_RendererAPI->Clear();
		}
		static void Dispatch(uint32_t x, uint32_t y, uint32_t z) {
			s_RendererAPI->Dispatch(x,y,z);
		}
		
		static void InsertBarrier(const BarrierDomain& barrier){
			s_RendererAPI->InsertBarrier(barrier);
		}
		
		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawIndexed(VertexArray* vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount = 0)
		{
			s_RendererAPI->DrawArrays(vertexArray, vertexCount);
		}

		// 鍋忕Щ缁樺埗鐨勬柟娉?
		static void DrawIndexedWithOffset(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset)
		{
			s_RendererAPI->DrawIndexedWithOffset(vertexArray, indexOffset, indexCount, vertexOffset);
		}

		// 瀹炰緥鍖栫粯鍒?
		static void DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexCount, uint32_t instanceCount)
		{
			s_RendererAPI->DrawIndexedInstanced(vertexArray, indexCount, instanceCount);
		}

		static void DrawIndexedInstanced(VertexArray* vertexArray, 
			uint32_t indexCount, uint32_t instanceCount)
		{
			s_RendererAPI->DrawIndexedInstanced(vertexArray, indexCount, instanceCount);
		}

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;
	};
} 
