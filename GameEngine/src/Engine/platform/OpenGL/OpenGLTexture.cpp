#include "pch.h"
#include "Engine/platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>
#include <cstring> // for memcpy

namespace Engine {
	GLenum GLTextureFormat(TextureFormat format) {
		GLenum glformat = GL_RGB8;
		switch (format) {
		case TextureFormat::RGBA8:
			glformat = GL_RGBA8;

			break;
		case TextureFormat::RED_INTEGER:
			glformat = GL_R32I;

			break;
		case TextureFormat::DEPTH24STENCIL8:
			glformat = GL_DEPTH24_STENCIL8;
			break;
		case TextureFormat::SRGBA:
			glformat = GL_SRGB8_ALPHA8;
			break;
		}
		return glformat;

	};
	GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	};
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height,TextureFormat format,int samples)
		: m_Width(width), m_Height(height)
	{
		switch (format) {
			case TextureFormat::RGBA8:
				m_InternalFormat = GL_RGBA8;
				m_DataFormat = GL_RGBA;
				break;
			case TextureFormat::RED_INTEGER:
				m_InternalFormat = GL_R32I;
				m_DataFormat = GL_RED_INTEGER;
				break;
			case TextureFormat::DEPTH24STENCIL8:
				m_InternalFormat = GL_DEPTH24_STENCIL8;
				break;
		}
		bool multisample = samples > 1;
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path,TextureFormat format)
		: m_Path(path)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		

		//翻转

		stbi_uc* data = nullptr;
		stbi_set_flip_vertically_on_load(1);
		
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) {
			ENGINE_CORE_ERROR("stbi_load failed for path: {}, reason: {}", path, stbi_failure_reason());
		}
		ENGINE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;
		GLenum internalFormat =GLTextureFormat(format);
		GLenum dataFormat = 0;
		if (channels == 4)
		{
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			dataFormat = GL_RGB;
		}
		m_DataFormat = dataFormat;

		ENGINE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		ENGINE_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		ENGINE_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		ENGINE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		ENGINE_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
	// 从6张图片加载立方体贴图
	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& faces)
		: m_Width(0), m_Height(0)
	{
		ENGINE_CORE_ASSERT(faces.size() == 6, "Cubemap requires exactly 6 faces!");

		// 创建立方体贴图纹理
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		// 立方体贴图不需要翻转
		stbi_set_flip_vertically_on_load(false);

		// 加载6个面的纹理
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			int width, height, nrChannels;
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			
			if (data)
			{
				// 记录第一个面的尺寸
				if (i == 0) {
					m_Width = width;
					m_Height = height;
				}

				// 确定格式
				GLenum format = GL_RGB;
				if (nrChannels == 4)
					format = GL_RGBA;
				else if (nrChannels == 3)
					format = GL_RGB;

				// 加载到立方体贴图的第 i 个面
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				
				ENGINE_CORE_INFO("Loaded cubemap face {}: {}", i, faces[i]);
			}
			else
			{
				ENGINE_CORE_ERROR("Cubemap texture failed to load at path: {}", faces[i]);
			}
			stbi_image_free(data);
		}
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		

		
	}

	// 从单张图片加载（简化实现，可用于天空盒的简单测试）
	OpenGLTextureCube::OpenGLTextureCube(const std::string& path, TextureFormat format)
		: m_Path(path), m_Width(0), m_Height(0)
	{
		// 创建立方体贴图纹理
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		// 立方体贴图不需要翻转
		stbi_set_flip_vertically_on_load(false);

		// 加载图片
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		
		if (data)
		{
			m_Width = width;
			m_Height = height;

			// 确定格式
			GLenum glFormat = GL_RGB;
			if (nrChannels == 4)
				glFormat = GL_RGBA;
			else if (nrChannels == 3)
				glFormat = GL_RGB;

			m_DataFormat = glFormat;
			m_InternalFormat = (nrChannels == 4) ? GL_RGBA8 : GL_RGB8;

			// 将同一张图片应用到立方体的所有6个面（仅用于测试）
			for (unsigned int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}

			ENGINE_CORE_INFO("Loaded cubemap from single image: {}", path);
		}
		else
		{
			ENGINE_CORE_ERROR("Cubemap texture failed to load at path: {}", path);
		}
		stbi_image_free(data);

		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::SetData(void* data, uint32_t size)
	{
		// 立方体贴图的SetData实现（如需动态更新）
		ENGINE_CORE_WARN("OpenGLTextureCube::SetData not fully implemented");
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
