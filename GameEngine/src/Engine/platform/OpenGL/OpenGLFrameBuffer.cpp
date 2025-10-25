#include "pch.h"
#include "Engine/Platform/OpenGL/OpenGLFramebuffer.h"


namespace Engine {

	static const uint32_t s_MaxFramebufferSize = 8192;//静态函数和常量只对当前编译单元可见
	GLenum GLTextureFormat(TextureFormat format);
	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static GLenum TextureFormatToGL(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA8:       return GL_RGBA8;
			case TextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			ENGINE_CORE_ASSERT(false,"Unown Framebuffer Format");
			return 0;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FBOID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_FBOID)
		{
			glDeleteFramebuffers(1, &m_FBOID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_FBOID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

		bool multisample = m_Specification.Samples > 1;
		GLenum multi= multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Ref<Texture>colortexture = Texture2D::Create(m_Specification.Width, m_Specification.Height, m_ColorAttachmentSpecifications[i].TextureFormat,1);
				m_RenderTextures.push_back(colortexture);
				m_ColorAttachments[i] = colortexture->GetRendererID();
				
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, multi, m_ColorAttachments[i], 0);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != TextureFormat::None)
		{
			Ref<Texture>depthtexture = Texture2D::Create(m_Specification.Width, m_Specification.Height, m_DepthAttachmentSpecification.TextureFormat, 1);
			m_RenderTextures.push_back(depthtexture);
			m_DepthAttachment = depthtexture->GetRendererID();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,multi, m_DepthAttachment, 0);
		}

		if (m_ColorAttachments.size() > 1)
		{
			ENGINE_CORE_ASSERT(m_ColorAttachments.size() <= 4, "frameColorAttachment more than 4");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		ENGINE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			ENGINE_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		//绑定FBO，保证能该读取帧缓冲数据
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOID);
		ENGINE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(),"Can't readPix from a frameAttachment out of range");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		//表示未读取到值
		int pixelData=-1;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		return pixelData;

	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		ENGINE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Can't read frameAttachment out of range");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			GLTextureFormat(spec.TextureFormat), GL_INT, &value);
	}
	void OpenGLFramebuffer::ClearColorAttachments(int value)
	{
		for(uint32_t i=0;i< m_ColorAttachments.size();i++){
			auto& spec = m_ColorAttachmentSpecifications[i];
			glClearTexImage(m_ColorAttachments[i], 0,
				GLTextureFormat(spec.TextureFormat), GL_INT, &value);
		}
	
	}

	void OpenGLFramebuffer::ColorMask(bool writeColor)
	{
		if (writeColor) {
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
		else {
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
	}

	OpenGLFrameBufferCube::OpenGLFrameBufferCube(TextureCube* textureCube)
	{
		m_textureCube = textureCube;
		glCreateFramebuffers(1, &m_FBOID);
		ReCreateDepthStencilBuffer();
	}

	OpenGLFrameBufferCube::~OpenGLFrameBufferCube()
	{
		if (m_FBOID != 0)
		{
			glDeleteFramebuffers(1, &m_FBOID);
		}
		if (m_DepthAttachment != 0)
		{
			glDeleteTextures(1, &m_DepthAttachment);
		}
	}

	void OpenGLFrameBufferCube::Bind()
	{
		ENGINE_CORE_ASSERT(m_FBOID != 0, "Framebuffer not initialized!");
		ENGINE_CORE_ASSERT(m_textureCube != nullptr, "TextureCube is null!");
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
		glViewport(0, 0, m_textureCube->GetWidth(), m_textureCube->GetHeight());
	}

	void OpenGLFrameBufferCube::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBufferCube::ReSetCubeRenderTexture(TextureCube* textureCube)
	{
		ENGINE_CORE_ASSERT(textureCube != nullptr, "TextureCube cannot be null!");
		m_textureCube = textureCube;
		ReCreateDepthStencilBuffer();
	}

	void OpenGLFrameBufferCube::SwitchCubeFaceRenderTarget(uint8_t id)
	{
		ENGINE_CORE_ASSERT(id < 6, "Cube face ID must be between 0 and 5!");
		ENGINE_CORE_ASSERT(m_textureCube != nullptr, "TextureCube is null!");
		
		// 使用DSA切换立方体贴图面
		glNamedFramebufferTextureLayer(m_FBOID, GL_COLOR_ATTACHMENT0, m_textureCube->GetRendererID(), 0, id);
		
		// 使用DSA检查framebuffer状态
		GLenum status = glCheckNamedFramebufferStatus(m_FBOID, GL_FRAMEBUFFER);
		ENGINE_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete after switching cube face!");
	}

	void OpenGLFrameBufferCube::ReCreateDepthStencilBuffer()
	{
		ENGINE_CORE_ASSERT(m_textureCube != nullptr, "TextureCube is null!");
		
		// 删除旧的深度缓冲区
		if (m_DepthAttachment != 0) {
			glDeleteTextures(1, &m_DepthAttachment);
			m_DepthAttachment = 0;
		}
		
		uint32_t width = m_textureCube->GetWidth();
		uint32_t height = m_textureCube->GetHeight();
		
		ENGINE_CORE_ASSERT(width > 0 && height > 0, "Invalid texture dimensions!");
		
		// 使用DSA创建深度缓冲区
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glTextureStorage2D(m_DepthAttachment, 1, GL_DEPTH24_STENCIL8, width, height);
		
		// 使用DSA设置纹理参数
		glTextureParameteri(m_DepthAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_DepthAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_DepthAttachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_DepthAttachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		// 使用DSA绑定到framebuffer
		glNamedFramebufferTexture(m_FBOID, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment);
		
		// 使用DSA检查framebuffer状态
		GLenum status = glCheckNamedFramebufferStatus(m_FBOID, GL_FRAMEBUFFER);
		ENGINE_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete after depth buffer creation!");
	}

	void OpenGLFrameBufferCube::ClearRenderTargetDepthStencil()
	{
		ENGINE_CORE_ASSERT(m_FBOID != 0, "Framebuffer not initialized!");
		

		GLfloat clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		GLfloat clearDepth = 1.0f;
		GLint clearStencil = 0;
		
		// 使用DSA清除当前面的颜色缓冲区
		glClearNamedFramebufferfv(m_FBOID, GL_COLOR, 0, clearColor);
		// 使用DSA清除当前面的深度缓冲区
		glClearNamedFramebufferfv(m_FBOID, GL_DEPTH, 0, &clearDepth);
		// 使用DSA清除当前面的模板缓冲区
		glClearNamedFramebufferiv(m_FBOID, GL_STENCIL, 0, &clearStencil);
	}

	bool OpenGLFrameBufferCube::IsComplete() const
	{
		ENGINE_CORE_ASSERT(m_FBOID != 0, "Framebuffer not initialized!");
		GLenum status = glCheckNamedFramebufferStatus(m_FBOID, GL_FRAMEBUFFER);
		return status == GL_FRAMEBUFFER_COMPLETE;
	}

}
