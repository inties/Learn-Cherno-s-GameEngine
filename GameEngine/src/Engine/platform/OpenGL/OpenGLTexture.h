//steam from Hazel
#pragma once

#include "Engine/Renderer/Texture.h"

#include <glad/glad.h>

namespace Engine {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		//constTexture表示纹理尺寸和mipmap层级不可变
		OpenGLTexture2D(uint32_t width, uint32_t height,TextureFormat format=TextureFormat::RGBA8,int samples=1);
		OpenGLTexture2D(const std::string& path, TextureFormat format = TextureFormat::RGB8);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width;  }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		
		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
	class OpenGLTextureCube : public TextureCube
	{
	public:
		// 从6张图片路径加载立方体贴图（按照 +X, -X, +Y, -Y, +Z, -Z 顺序）
		OpenGLTextureCube(const std::vector<std::string>& faces,TextureFormat format=TextureFormat::SRGBA);
		// 从单张等距柱状投影图加载（简化版本，暂不实现）
		OpenGLTextureCube(const std::string& path, TextureFormat format = TextureFormat::SRGBA);
		virtual ~OpenGLTextureCube();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
	

}
