#include"pch.h"
#include"Input.h"
#include "platform/WindowsInput.h"
namespace Engine {
	Input* Input::s_Instance = nullptr;// ��̬��Ա�����Ķ���
	
	Input* Input::Create() {
		return new WindowsInput();
	}
}