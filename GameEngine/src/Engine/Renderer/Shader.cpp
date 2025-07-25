#include "pch.h"
#include "Shader.h"

#include "Engine/platform/OpenGL/OpenGLShader.h"

namespace Engine
{
	std::shared_ptr<Shader> Shader::Create(const std::string& filepath)
	{
		return std::make_shared<OpenGLShader>(filepath);
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc, true);
	}

	std::shared_ptr<Shader> Shader::CreateFromFiles(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath)
	{
		return std::make_shared<OpenGLShader>(name, vertexFilePath, fragmentFilePath);
	}
} 