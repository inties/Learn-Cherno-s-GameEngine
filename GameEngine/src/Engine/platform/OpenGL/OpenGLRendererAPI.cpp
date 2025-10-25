#include "pch.h"
#include "OpenGLRendererAPI.h"
#include "Engine/Renderer/Buffer.h"
#include <glad/glad.h>

namespace Engine
{
	GLenum GLTypologyType(TopologyType topology) {
		switch (topology)
		{
			case TopologyType::TriangleList:
				return GL_TRIANGLES;
			case TopologyType::TriangleStrip:
				return GL_TRIANGLE_STRIP;
			case TopologyType::Point:
				return GL_POINTS;
			case TopologyType::LineList:
				return GL_LINES;
			case TopologyType::LineStrip:
				return GL_LINE_STRIP;
			default:
				ENGINE_CORE_ERROR("Unknown topology type: {}", static_cast<int>(topology));
				return GL_TRIANGLES; // 默认返回三角形
		}
	}
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetWildFrame(bool enable)
	{
		// 设置线框模式
		if (enable) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void OpenGLRendererAPI::Dispatch(uint32_t x, uint32_t y, uint32_t z)
	{
		glDispatchCompute(x,y,z);
	}

	void OpenGLRendererAPI::InsertBarrier(const BarrierDomain& barrier) const
	{
		
		switch (barrier)
		{
		case BarrierDomain::ComputeWriteToComputeRead:
			glMemoryBarrier(
				GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
				GL_SHADER_STORAGE_BARRIER_BIT
			);
			break;

		case BarrierDomain::ComputeWriteToGraphicsRead:
			glMemoryBarrier(
				GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
				GL_SHADER_STORAGE_BARRIER_BIT |
				GL_TEXTURE_FETCH_BARRIER_BIT
			);
			break;

		case BarrierDomain::RenderTargetWriteToSample:
			glMemoryBarrier(
				GL_FRAMEBUFFER_BARRIER_BIT |
				GL_TEXTURE_FETCH_BARRIER_BIT
			);
			break;
		case BarrierDomain::ComputeWriteToRenderTarget:
			glMemoryBarrier(
				GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
				GL_FRAMEBUFFER_BARRIER_BIT
			);
		}

		
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		uint32_t count = vertexCount ? vertexCount : vertexArray->GetVertexCount();
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		glDrawArrays(mode, 0, count);
	}

	void OpenGLRendererAPI::DrawIndexedWithOffset(const std::shared_ptr<VertexArray>& vertexArray, 
		uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset)
	{
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		// 使用glDrawElementsBaseVertex支持偏移绘制
		glDrawElementsBaseVertex(mode, indexCount, GL_UNSIGNED_INT, 
			(void*)(indexOffset * sizeof(uint32_t)), vertexOffset);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, 
		uint32_t indexCount, uint32_t instanceCount)
	{
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(const VertexArray* vertexArray, 
		uint32_t indexCount, uint32_t instanceCount)
	{
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, nullptr, instanceCount);
	}
} 
