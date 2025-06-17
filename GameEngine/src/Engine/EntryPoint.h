#pragma once

// EntryPoint.h 应该只在客户端应用程序中使用
// 这个头文件提供 main 函数的实现，客户端只需要实现 createApplication

#include"Engine.h"
#include "Application.h"
#include "log.h"

#include<iostream>

extern Engine::Application* Engine::createApplication();

int main() {
	// 初始化引擎日志系统
	
	Engine::Log::init();
	ENGINE_CORE_INFO("Engine Initialized");
	ENGINE_INFO("Client Initialized");
	Engine::MouseMoveEvent e(1.0, 2.0);
	std::string str = e.ToString();
	ENGINE_TRACE(str);
	// 创建客户端应用程序实例
	Engine::Application* app = Engine::createApplication();
	app->run();
	delete app;
	return 0;
}