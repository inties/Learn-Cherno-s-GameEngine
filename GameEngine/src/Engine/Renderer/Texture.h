//steal form hazel
#pragma once

#include "pch.h"



namespace Engine {

	enum class TextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RGBA16,
		RGBA32,
		
		RED_INTEGER,
		RGB8,
		RGB16,
		RGB32,
		SRGBA,
		// Single channel
		RED,
		RED32F,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};
	enum class TextureAccess
	{
		ReadOnly,
		WriteOnly,
		ReadWrite
	};
	struct ImageBindDesc
	{
		uint32_t binding = 0;          // 对应 GLSL layout(binding = X)
		uint32_t mipLevel = 0;         // 要暴露哪一层mip
		uint32_t layer = 0;            // 对2D数组/3D分层纹理: 哪一层
		bool layered = false;          // true时绑定整个layered texture (array/3D)
		TextureAccess access = TextureAccess::ReadOnly;

		// 这个必须和GLSL中的layout(format)一致
		//GLenum internalFormatGL = GL_RGBA8; // e.g. GL_RGBA16F, GL_R32UI, ...
	};
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void BindAsImage(const ImageBindDesc& desc)const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Clear() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		

		
		virtual bool operator==(const Texture& other) const = 0;
	
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8, int sample = 1);
		static Ref<Texture2D> Create(const std::string& path);

		static Scope<Texture2D> CreateTexScope(const std::string& path,TextureFormat format,bool enable_mipmap=false,bool HDRsource=false);
	};

	class TextureCube : public Texture {
	public:
		// 6张图片创建立方体贴图
		static Scope<TextureCube> Create(const std::vector<std::string>& faces);
		// 从单张图片创建立方体贴图（所有面使用同一张图
		static Scope<TextureCube> Create(const std::string& path);
		static Scope<TextureCube> Create(TextureFormat, uint32_t width, uint32_t height);
	};

}
