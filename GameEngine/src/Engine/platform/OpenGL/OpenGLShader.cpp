#include "pch.h"
#include "OpenGLShader.h"
#include "Engine/log.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		ENGINE_CORE_ERROR("Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath, const ShaderDesc& desc)
	{
		if (desc.type == ShaderType::compute) {
			std::string source = ReadFile(filepath);
			// 1. ���������ɫ��
			GLuint cs = CompileShader(GL_COMPUTE_SHADER, source.c_str());
			if (cs == 0)
			{
				// ����ʧ��
				return;
			}

			// 2. ����program������
			GLuint program = glCreateProgram();
			glAttachShader(program, cs);
			glLinkProgram(program);

			// ���ǲ�����Ҫcs�����ˣ�������ɾ
			glDetachShader(program, cs);
			glDeleteShader(cs);

			// 3. �������״̬
			GLint linked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &linked);
			if (!linked)
			{
				GLint logLen = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

				std::string log;
				log.resize(logLen > 1 ? logLen : 1);

				glGetProgramInfoLog(program, logLen, nullptr, log.data());

				std::cerr << "[GL] Compute program link failed:\n"
					<< log << std::endl;

				glDeleteProgram(program);
				return;
			}
			m_program = program;
		}
		else {
			ENGINE_CORE_ERROR("unsupported shader type(OpenGLShader)");
		}
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		std::string vertexSrc = ReadFile(vertexFilePath);
		std::string fragmentSrc = ReadFile(fragmentFilePath);
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, bool isSource)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_program);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				ENGINE_CORE_ERROR("Could not read from file '{}'", filepath);
			}
		}
		else
		{
			ENGINE_CORE_ERROR("Could not open file '{}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			if (eol == std::string::npos)
			{
				ENGINE_CORE_ERROR("Syntax error");
				break;
			}
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			if (ShaderTypeFromString(type) == 0)
			{
				ENGINE_CORE_ERROR("Invalid shader type specified");
				break;
			}

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			if (nextLinePos == std::string::npos)
			{
				ENGINE_CORE_ERROR("Syntax error");
				break;
			}
			pos = source.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		if (shaderSources.size() > 2)
		{
			ENGINE_CORE_ERROR("We only support 2 shaders for now");
			return;
		}
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				ENGINE_CORE_ERROR("{}", infoLog.data());
				ENGINE_CORE_ERROR("Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_program = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			ENGINE_CORE_ERROR("{}", infoLog.data());
			ENGINE_CORE_ERROR("Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	GLint OpenGLShader::CompileShader(GLenum type, const char* src)
	{
			
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		// ������״̬
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLint logLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

			std::string log;
			log.resize(logLen > 1 ? logLen : 1);

			glGetShaderInfoLog(shader, logLen, nullptr, log.data());

			std::cerr << "[GL] Shader compilation failed ("
				<< (type == GL_COMPUTE_SHADER ? "COMPUTE" :
					type == GL_VERTEX_SHADER ? "VERTEX" :
					type == GL_FRAGMENT_SHADER ? "FRAGMENT" :
					"OTHER")
				<< "):\n"
				<< log << std::endl;

			glDeleteShader(shader);
			return 0;
		}

		return shader;
		
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_program);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_program, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
} 
