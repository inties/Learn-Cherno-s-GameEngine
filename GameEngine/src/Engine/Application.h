#pragma once
#include "core.h"

namespace Engine {
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void run();
	};
	
	// 由客户端实现
	Application* createApplication();
}

