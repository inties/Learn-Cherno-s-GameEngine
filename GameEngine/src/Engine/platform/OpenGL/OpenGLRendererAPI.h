#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;
		void SetWildFrame (bool enable)override;
		void Dispatch(uint32_t x, uint32_t y, uint32_t z)override;
		void InsertBarrier(const BarrierDomain& barrier)const override;
		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount = 0) override;
		void DrawIndexedWithOffset(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset) override;
		void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0)override;
		void SetBlendState(const BlendDesc& desc)override;
		// 深度/模板设置
		void SetDepthStencilState(const DepthStencilDesc& desc) override;
		// 实例化绘制
		void DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, 
			uint32_t indexCount, uint32_t instanceCount) override;
		void DrawIndexedInstanced(const VertexArray* vertexArray, 
			uint32_t indexCount, uint32_t instanceCount) override;
	};
} 
