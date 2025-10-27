#pragma once

#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/Texture.h"

namespace Engine {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;
		
		void ClearColorAttachments(int value)override;
		void ColorMask(bool = true) override;
		//获取指定序号的纹理附件
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { ENGINE_CORE_ASSERT(index < m_ColorAttachments.size(),"Can't get ColorAttachment Out of Range"); return m_ColorAttachments[index]; }
		Ref<Texture>GetRenderTexture(uint32_t index = 0)const override {
			ENGINE_CORE_ASSERT(index < m_ColorAttachments.size(), "Can't get ColorAttachment Out of Range");
			return m_RenderTextures[index];
		}
		Ref<Texture>GetDepth()const override {
			return m_RenderTextures.back();
		}
		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_FBOID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = TextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		std::vector<Ref<Texture>>m_RenderTextures;
		uint32_t m_DepthAttachment = 0;
	};

	/**
	 * @brief OpenGL立方体贴图帧缓冲区实现 (DSA版本)
	 * 
	 * 使用Direct State Access (DSA) API实现，提供更好的性能和更简洁的代码
	 * DSA允许直接操作OpenGL对象而不需要绑定到当前上下文
	 * 
	 * 主要DSA函数使用：
	 * - glCreateFramebuffers() - 创建framebuffer
	 * - glCreateTextures() - 创建纹理
	 * - glNamedFramebufferTexture2D() - 绑定纹理到framebuffer
	 * - glClearNamedFramebuffer*() - 清除framebuffer
	 * - glCheckNamedFramebufferStatus() - 检查framebuffer状态
	 */
	class OpenGLFrameBufferCube final:public Framebuffer {
	public:

		OpenGLFrameBufferCube(TextureCube*);

		~OpenGLFrameBufferCube();
		
		void Bind() override;
		
		void Unbind() override;
		
		void ReSetCubeRenderTexture(TextureCube*);
		void SwitchCubeFaceRenderTarget(uint8_t id);
		
		void ClearRenderTargetDepthStencil();
	
		void ReCreateDepthStencilBuffer();
		
		// 获取相关信息的公共方法
		uint32_t GetFramebufferID() const { return m_FBOID; }
		TextureCube* GetTextureCube() const { return m_textureCube; }
		uint32_t GetDepthAttachmentID() const { return m_DepthAttachment; }
		
		bool IsComplete() const;

		//以下函数不需要实现，但需要提供默认返回值
		void Resize(uint32_t width, uint32_t height) override {}
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override { return -1; }

		void ClearAttachment(uint32_t attachmentIndex, int value) override {}
		void ClearColorAttachments(int value) override {}

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return 0; }

		//获取颜色纹理附件对应的Texture
		Ref<Texture>GetRenderTexture(uint32_t index = 0) const override { return nullptr; }

		const FramebufferSpecification& GetSpecification() const override { 
			static FramebufferSpecification emptySpec;
			return emptySpec; 
		}
	private:

		uint32_t m_FBOID = 0;
		TextureCube* m_textureCube = nullptr;
		uint32_t m_DepthAttachment = 0;
	};

}
