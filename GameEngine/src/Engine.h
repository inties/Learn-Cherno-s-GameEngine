#pragma once

// GameEngine 核心头文件
// 客户端应用程序包含此文件来使用引擎功能

// 引擎核心功能
#include "Engine/core.h"
#include "Engine/Application.h"
#include "Engine/log.h"
#include "Engine/Event.h"

// OpenGL相关依赖
#include <glad/glad.h>
//#define GLEW_STATIC
//#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

// 客户端入口点（可选包含）
// #include "Engine/EntryPoint.h"