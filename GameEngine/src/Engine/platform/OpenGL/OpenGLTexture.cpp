#include "pch.h"
#include "Engine/platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>
#include <cstring> // for memcpy

namespace Engine {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		// HZ_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		HZ_PROFILE_FUNCTION();

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
		HZ_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		ENGINE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}