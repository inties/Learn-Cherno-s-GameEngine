#include "pch.h"
#include "VertexArray.h"

#include "Engine/platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	Ref<VertexArray> VertexArray::Create()
	{
		return std::make_shared<OpenGLVertexArray>();
	}
} 