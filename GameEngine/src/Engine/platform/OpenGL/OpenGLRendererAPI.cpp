#include "pch.h"
#include "OpenGLRendererAPI.h"
#include "Engine/Renderer/Buffer.h"
#include <glad/glad.h>

namespace Engine
{
	GLenum ConvertGLBlendFactor(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero: return GL_ZERO;
		case BlendFactor::One: return GL_ONE;
		case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha: return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::SrcColor: return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor: return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
		default:
			assert(false && "Unknown blend factor");
			return GL_ONE;
		}
	}
	GLenum ConvertGLBlendOp(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add: return GL_FUNC_ADD;
		case BlendOp::Subtract: return GL_FUNC_SUBTRACT;
		case BlendOp::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOp::Min: return GL_MIN;
		case BlendOp::Max: return GL_MAX;
		default:
			assert(false && "Unknown blend operation");
			return GL_FUNC_ADD;
		}
	}

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

	// Convert CompareOp to GL compare function
	static GLenum ConvertGLCompareOp(CompareOp op)
	{
		switch (op)
		{
		case CompareOp::Disabled: return GL_ALWAYS; // if disabled we'll disable test anyway
		case CompareOp::Never: return GL_NEVER;
		case CompareOp::Less: return GL_LESS;
		case CompareOp::Equal: return GL_EQUAL;
		case CompareOp::LessEqual: return GL_LEQUAL;
		case CompareOp::Greater: return GL_GREATER;
		case CompareOp::NotEqual: return GL_NOTEQUAL;
		case CompareOp::GreaterEqual:return GL_GEQUAL;
		case CompareOp::Always: return GL_ALWAYS;
		default:
			return GL_LESS;
		}
	}

	static GLenum ConvertGLStencilOp(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep: return GL_KEEP;
		case StencilOp::Zero: return GL_ZERO;
		case StencilOp::Replace:return GL_REPLACE;
		case StencilOp::IncrSat:return GL_INCR;
		case StencilOp::DecrSat:return GL_DECR;
		case StencilOp::Invert: return GL_INVERT;
		case StencilOp::Incr: return GL_INCR_WRAP;
		case StencilOp::Decr: return GL_DECR_WRAP;
		default: return GL_KEEP;
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

	void OpenGLRendererAPI::SetBlendState(const BlendDesc& desc)
	{
		if (desc.alphaToCoverageEnable) {
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}
		else {
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}

		// OpenGL需要为每个渲染目标单独设置（通过帧缓冲区附件）
		for (int i =0; i < BlendDesc::MAX_RENDER_TARGETS; ++i) {
			if (i >0 && !desc.independentBlendEnable) {
				// 如果非独立混合，所有RT使用第一个的设置
				break;
			}

			const auto& rt = desc.renderTarget[i];
			if (rt.blendEnable) {
				glEnablei(GL_BLEND, i);
				glBlendFuncSeparatei(i,
					ConvertGLBlendFactor(rt.srcBlend), ConvertGLBlendFactor(rt.dstBlend),
					ConvertGLBlendFactor(rt.srcBlendAlpha), ConvertGLBlendFactor(rt.dstBlendAlpha));
				glBlendEquationSeparatei(i,
					ConvertGLBlendOp(rt.blendOp), ConvertGLBlendOp(rt.blendOpAlpha));
			}
			else {
				glDisablei(GL_BLEND, i);
			}

			// 设置颜色写入掩码
			glColorMaski(i, rt.colorWriteMask.red, rt.colorWriteMask.green,
				rt.colorWriteMask.blue, rt.colorWriteMask.alpha);
		}

		// 设置混合常量颜色
		glBlendColor(desc.blendFactor[0], desc.blendFactor[1],
			desc.blendFactor[2], desc.blendFactor[3]);
	}

	void OpenGLRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		uint32_t count = vertexCount ? vertexCount : vertexArray->GetVertexCount();
		vertexArray->Bind();
		GLenum mode = GLTypologyType(vertexArray->GetTopologyType());
		glDrawArrays(mode,0, count);
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

	void OpenGLRendererAPI::SetDepthStencilState(const DepthStencilDesc& desc)
	{
		// Depth
		if (desc.depthTestEnable) {
			glEnable(GL_DEPTH_TEST);
			// depth write
			glDepthMask(desc.depthWriteEnable ? GL_TRUE : GL_FALSE);
			// depth func
			glDepthFunc(ConvertGLCompareOp(desc.depthCompare));
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		// Stencil
		if (desc.stencilEnable) {
			glEnable(GL_STENCIL_TEST);
			// front face
			glStencilFuncSeparate(GL_FRONT,
				ConvertGLCompareOp(desc.front.failOp == StencilOp::Keep ? CompareOp::Always : CompareOp::Less), // placeholder
				GL_ALWAYS,0xFF);
			// We'll actually set func using provided reference and readMask below

			glStencilFuncSeparate(GL_FRONT,
				ConvertGLCompareOp(CompareOp::Always), // override later
				static_cast<GLint>(desc.front.reference),
				desc.front.readMask);

			glStencilOpSeparate(GL_FRONT,
				ConvertGLStencilOp(desc.front.failOp),
				ConvertGLStencilOp(desc.front.depthFailOp),
				ConvertGLStencilOp(desc.front.passOp));
			glStencilMaskSeparate(GL_FRONT, desc.front.writeMask);

			// back face
			glStencilFuncSeparate(GL_BACK,
				ConvertGLCompareOp(CompareOp::Always),
				static_cast<GLint>(desc.back.reference),
				desc.back.readMask);

			glStencilOpSeparate(GL_BACK,
				ConvertGLStencilOp(desc.back.failOp),
				ConvertGLStencilOp(desc.back.depthFailOp),
				ConvertGLStencilOp(desc.back.passOp));
			glStencilMaskSeparate(GL_BACK, desc.back.writeMask);
		}
		else {
			glDisable(GL_STENCIL_TEST);
		}
	}
} 
