#include "pch.h"
#include "Engine/platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>
#include <cstring> // for memcpy

namespace Engine {
	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}
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

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		// 使用线程安全的stbi_set_flip_vertically_on_load_thread替代全局设置
		// 这避免了多线程环境下的数据竞争问题
#ifdef STBI_THREAD_LOCAL
		stbi_set_flip_vertically_on_load_thread(1);
#else
		// 如果不支持线程局部变量，手动翻转
		bool manual_flip = true;
#endif
		stbi_uc* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
#ifdef STBI_THREAD_LOCAL
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
#else
			// 不设置全局翻转，避免线程竞争
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			if (!data) {
				ENGINE_CORE_ERROR("stbi_load failed for path: {}, reason: {}", path, stbi_failure_reason());
			}

#endif
		}
		ENGINE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

#ifndef STBI_THREAD_LOCAL
		// 手动翻转图像数据（从底部到顶部）
		if (manual_flip && data) {
			int bytes_per_pixel = channels;
			int row_bytes = width * bytes_per_pixel;
			stbi_uc* temp_row = new stbi_uc[row_bytes];
			
			for (int y = 0; y < height / 2; ++y) {
				stbi_uc* row1 = data + y * row_bytes;
				stbi_uc* row2 = data + (height - 1 - y) * row_bytes;
				
				// 交换行
				memcpy(temp_row, row1, row_bytes);
				memcpy(row1, row2, row_bytes);
				memcpy(row2, temp_row, row_bytes);
			}
			
			delete[] temp_row;
		}
#endif

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		ENGINE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
