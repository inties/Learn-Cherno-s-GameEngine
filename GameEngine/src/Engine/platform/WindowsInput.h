#pragma once
#include"Engine/Input.h"
#include<glfw/glfw3.h>
namespace Engine {
	class ENGINE_API WindowsInput :public Input {
	public:
		
		virtual bool IsKeyPressed(int keycode)override;
		virtual bool IsMouseButtonPressed(int mousecode)override;
		virtual float GetXpos()override;
		virtual float GetYpos()override;
		virtual std::pair<float, float> GetMousePos()override;
	private:
		WindowsInput();
		friend class Input;  // 允许Input类访问private构造函数
	};


	

	
}