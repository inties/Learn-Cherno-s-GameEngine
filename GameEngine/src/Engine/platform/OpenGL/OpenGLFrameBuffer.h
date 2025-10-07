#pragma once
#pragma once

#include "Engine/Renderer/Framebuffer.h"

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

		//获取指定序号的纹理附件
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { ENGINE_CORE_ASSERT(index < m_ColorAttachments.size(),"Can't get ColorAttachment Out of Range"); return m_ColorAttachments[index]; }
		Ref<Texture>GetRenderTexture(uint32_t index = 0)const override {
			ENGINE_CORE_ASSERT(index < m_ColorAttachments.size(), "Can't get ColorAttachment Out of Range");
			return m_RenderTextures[index];
		}
		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = TextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		std::vector<Ref<Texture>>m_RenderTextures;
		uint32_t m_DepthAttachment = 0;
	};

}
