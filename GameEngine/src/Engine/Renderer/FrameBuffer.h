#pragma once
#include "pch.h"
#include "Engine/core.h"
#include "Engine/Renderer/Texture.h"
namespace Engine {

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(TextureFormat format)
			: TextureFormat(format) {
		}

		TextureFormat TextureFormat = TextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual void ClearColorAttachments(int value)=0;
		virtual void ColorMask(bool = true)=0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		//获取颜色纹理附件对应的Texture
		virtual Ref<Texture>GetRenderTexture(uint32_t index = 0)const = 0;

		
		virtual Ref<Texture>GetDepth()const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Engine::Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};


}
