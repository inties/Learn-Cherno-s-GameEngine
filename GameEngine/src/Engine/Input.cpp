#include"pch.h"
#include"Input.h"
#include "platform/WindowsInput.h"
namespace Engine {
	Input* Input::s_Instance = nullptr;// 静态成员变量的定义
	
	Input* Input::Create() {
		return new WindowsInput();
	}
}