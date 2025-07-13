#include "pch.h"
#include "VertexArray.h"

#include "Engine/platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		return std::make_shared<OpenGLVertexArray>();
	}
} 