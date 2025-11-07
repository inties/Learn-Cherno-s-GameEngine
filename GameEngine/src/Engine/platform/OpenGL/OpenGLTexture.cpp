#include "pch.h"
#include "Engine/platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>
#include <cstring> // for memcpy
#include <algorithm>
#include <cmath>

namespace Engine {
	struct ClearParams {
		GLenum format;   // glClearTexImage 的 'format' 参数
		GLenum type;     // glClearTexImage 的 'type' 参数
		const void* data; // 指向实际清除值
	};
	GLenum GLTextureFormat(TextureFormat format) {
		GLenum glformat = GL_RGB8;
		switch (format) {
		case TextureFormat::RGBA8:
			glformat = GL_RGBA8;
			break;
		case TextureFormat::RGB8:
			glformat = GL_RGB8;
			break;
		case TextureFormat::RGBA16:
			glformat = GL_RGBA16F;
			break;
		case TextureFormat::RGBA32:
			glformat = GL_RGBA32F;
			break;
		case TextureFormat::RGB16:
			glformat = GL_RGB16F;
			break;
		case TextureFormat::RGB32:
			glformat = GL_RGB32F;
			break;
		case TextureFormat::RED_INTEGER:
			glformat = GL_R32I;
			break;
		case TextureFormat::RED:
			glformat = GL_R8;
			break;
		case TextureFormat::RED32F:
			glformat = GL_R32F;
			break;
		case TextureFormat::SRGBA:
			glformat = GL_SRGB8_ALPHA8;
			break;
		case TextureFormat::DEPTH24STENCIL8:
			glformat = GL_DEPTH24_STENCIL8;
			break;
		}
		return glformat;
	};
	GLenum GLDataFormat(int channels) {
		if (channels == 4)
		{
			return GL_RGBA;
		}
		else if (channels == 3)
		{
			return GL_RGB;
		}
		else if (channels == 1) {
			return GL_RED;
		}
	}
	GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	};
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height,TextureFormat format,int samples,bool enable_mipmap)
		: m_Width(width), m_Height(height)
	{
		m_format = format;
		m_InternalFormat=GLTextureFormat(format);
		bool multisample = samples > 1;
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// 计算总的mipmap层级：floor(log2(max(w, h))) + 1
		int mipLevels = enable_mipmap ? (int)std::floor(std::log2((double)std::max(m_Width, m_Height))) + 1 : 1;
		glTextureStorage2D(m_RendererID, mipLevels, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, enable_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path,TextureFormat format,bool enable_mipmap,bool HDRsource)
		: m_Path(path)
	{
		ENGINE_PROFILE_FUNCTION();

		int width, height, channels;
		
		stbi_uc* data = nullptr;
		float* hdrdata = nullptr;
		stbi_set_flip_vertically_on_load(1);
		
		if (HDRsource) {
			hdrdata = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			if (!hdrdata) {
				ENGINE_CORE_ERROR("stbi_load failed for path: {}, reason: {}", path, stbi_failure_reason());
			}
		}
		else {
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			if (!data) {
				ENGINE_CORE_ERROR("stbi_load failed for path: {}, reason: {}", path, stbi_failure_reason());
			}
		}
		
		m_Width = width;
		m_Height = height;
		m_format = format;
		GLenum internalFormat = GLTextureFormat(format);
		GLenum dataFormat = GLDataFormat(channels);
		m_DataFormat = dataFormat;

		ENGINE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// 计算总的mipmap层级
		int mipLevels = enable_mipmap ? (int)std::floor(std::log2((double)std::max(m_Width, m_Height))) + 1 : 1;
		glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, enable_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		// 上传基础纹理数据
		if (HDRsource) {
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_FLOAT, hdrdata);
		}
		else {
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		}
		
		// 生成mipmap
		if (enable_mipmap)
		{
			glGenerateTextureMipmap(m_RendererID);
		}

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

		uint32_t bpp = 4; // 默认RGBA
		if (m_DataFormat == GL_RGBA) {
			bpp = 4;
		}
		else if (m_DataFormat == GL_RGB) {
			bpp = 3;
		}
		else if (m_DataFormat == GL_RED) {
			bpp = 1;
		}
		else if (m_DataFormat == GL_RED_INTEGER) {
			bpp = 4; // 32位整数
		}
		
		ENGINE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Clear()
	{
		ClearParams params{};
		bool supported = true;

		switch (m_format)  
		{
		case TextureFormat::RGBA8:
		{
			static const GLubyte clearColor[4] = { 0,0,0,1 };
			params = { GL_RGBA, GL_UNSIGNED_BYTE, clearColor };
		} break;

		case TextureFormat::RGB8:
		{
			static const GLubyte clearColor[3] = { 0,0,0 };
			params = { GL_RGB, GL_UNSIGNED_BYTE, clearColor };
		} break;

		case TextureFormat::SRGBA: // GL_SRGB8_ALPHA8
		{
			static const GLubyte clearColor[4] = { 0,0,0,1 };
			params = { GL_RGBA, GL_UNSIGNED_BYTE, clearColor };
		} break;

		case TextureFormat::RED: // GL_R8
		{
			static const GLubyte clearColor[1] = { 0 };
			params = { GL_RED, GL_UNSIGNED_BYTE, clearColor };
		} break;

		case TextureFormat::RGBA16: // GL_RGBA16F
		case TextureFormat::RGBA32: // GL_RGBA32F
		{
			static const GLfloat clearColor[4] = { 0.f,0.f,0.f,1.0f };
			params = { GL_RGBA, GL_FLOAT, clearColor };
		} break;

		case TextureFormat::RGB16: // GL_RGB16F
		case TextureFormat::RGB32: // GL_RGB32F
		{
			static const GLfloat clearColor[3] = { 0.f,0.f,0.f };
			params = { GL_RGB, GL_FLOAT, clearColor };
		} break;

		case TextureFormat::RED32F: // GL_R32F
		{
			static const GLfloat clearColor[1] = { 0.f };
			params = { GL_RED, GL_FLOAT, clearColor };
		} break;

		case TextureFormat::RED_INTEGER: // GL_R32I
		{
			static const GLint clearColor[1] = { 0 };
			params = { GL_RED_INTEGER, GL_INT, clearColor };
		} break;

		case TextureFormat::DEPTH24STENCIL8: // GL_DEPTH24_STENCIL8
		{
			// depth = 1.0f (all 1s in the 24 depth bits), stencil = 0
			static const GLuint clearValue[1] = { 0xFFFFFF00u };
			params = { GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, clearValue };
		} break;

		default:
			supported = false;
			break;
		}

		if (!supported)
		{
			
			return;
		}

		glClearTexImage(
			m_RendererID,
			0,                // mip level
			params.format,    // e.g. GL_RGBA / GL_RED_INTEGER / GL_DEPTH_STENCIL ...
			params.type,      // e.g. GL_FLOAT / GL_UNSIGNED_BYTE / GL_INT ...
			params.data       // 指向上面静态数组
		);
		glClearTexImage(m_RendererID, 0, params.format,params.type,params.data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		ENGINE_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
	void OpenGLTexture2D::BindAsImage(const ImageBindDesc& desc)const 
	{
		GLenum accessGL = GL_READ_ONLY;
		switch (desc.access)
		{
		case TextureAccess::ReadOnly:  accessGL = GL_READ_ONLY;  break;
		case TextureAccess::WriteOnly: accessGL = GL_WRITE_ONLY; break;
		case TextureAccess::ReadWrite: accessGL = GL_READ_WRITE; break;
		}

		glBindImageTexture(
			desc.binding,
			m_RendererID,
			desc.mipLevel,
			desc.layered ? GL_TRUE : GL_FALSE,
			desc.layer,
			accessGL,
			m_InternalFormat
		);
	}
	// 从6张图片加载立方体贴图
	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& faces,TextureFormat format)
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
				GLenum internalFormat = GLTextureFormat(format);
				GLenum dataFormat = GLDataFormat(nrChannels);

				// 加载到立方体贴图的第 i 个面
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				
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

	// 从单张图片加载
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
			GLenum glFormat = GLTextureFormat(format);

			// 将同一张图片应用到立方体的所有6个面（仅用于测试）
			for (unsigned int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, glFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

	OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		GLenum internalFormat = GLTextureFormat(format);
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		
		// 为整个立方体贴图分配存储空间
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
		
		// 使用DSA设置纹理参数
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
