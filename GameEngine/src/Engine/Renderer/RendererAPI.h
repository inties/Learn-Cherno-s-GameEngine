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
	// 混合因子
	enum class BlendFactor : unsigned char
	{
		Zero,
		One,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor
	};

	// 混合方程
	enum class BlendOp : unsigned char
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};
    // 单个渲染目标的混合设置
    struct RenderTargetBlendDesc
    {
        bool blendEnable = false;           // 是否启用混合
        BlendFactor srcBlend = BlendFactor::One;
        BlendFactor dstBlend = BlendFactor::Zero;
        BlendOp blendOp = BlendOp::Add;
        BlendFactor srcBlendAlpha = BlendFactor::One;
        BlendFactor dstBlendAlpha = BlendFactor::Zero;
        BlendOp blendOpAlpha = BlendOp::Add;

        // 颜色写入掩码 (RGBA)
        struct ColorWriteMask
        {
            bool red = true;
            bool green = true;
            bool blue = true;
            bool alpha = true;

            // 转换为位掩码格式（DX12风格）
            uint8_t ToMask() const {
                return (red ? 0x1 : 0) | (green ? 0x2 : 0) | (blue ? 0x4 : 0) | (alpha ? 0x8 : 0);
            }

            // 从位掩码构造
            static ColorWriteMask FromMask(uint8_t mask) {
                return { (mask & 0x1) != 0, (mask & 0x2) != 0, (mask & 0x4) != 0, (mask & 0x8) != 0 };
            }
        } colorWriteMask;
    };

    // 完整的混合状态描述
    struct BlendDesc
    {
        bool alphaToCoverageEnable = false;    // Alpha-to-Coverage
        bool independentBlendEnable = false;   // 每个RT独立混合设置

        // 多个渲染目标的混合设置（通常支持8个）
        static constexpr int MAX_RENDER_TARGETS = 8;
        RenderTargetBlendDesc renderTarget[MAX_RENDER_TARGETS];

        // 混合常量颜色（用于ConstantColor混合因子）
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        // 设置所有渲染目标使用相同的混合参数
        void SetAllRenderTargetsSame(const RenderTargetBlendDesc& desc) {
            for (int i = 0; i < MAX_RENDER_TARGETS; ++i) {
                renderTarget[i] = desc;
            }
            independentBlendEnable = false;
        }

        // 创建常用的混合模式
        static BlendDesc CreateOpaque() {
            BlendDesc desc;
            desc.SetAllRenderTargetsSame({
                false,  // blendEnable
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add,
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add
                });
            return desc;
        }

        static BlendDesc CreateAlphaBlend() {
            BlendDesc desc;
            desc.SetAllRenderTargetsSame({
                true,  // blendEnable
                BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add,
                BlendFactor::One, BlendFactor::Zero, BlendOp::Add
                });
            return desc;
        }

        static BlendDesc CreateAdditive() {
            BlendDesc desc;
            desc.SetAllRenderTargetsSame({
                true,  // blendEnable
                BlendFactor::SrcAlpha, BlendFactor::One, BlendOp::Add,
                BlendFactor::One, BlendFactor::One, BlendOp::Add
                });
            return desc;
        }
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
		virtual void SetBlendState(const BlendDesc& desc) = 0;
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
