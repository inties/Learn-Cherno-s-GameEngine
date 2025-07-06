#pragma once
#include"Engine/Input.h"
#include<glfw/glfw3.h>
namespace Engine {
	class ENGINE_API WindowsInput :public Input {
	public:
		WindowsInput();
		virtual bool IsKeyPressed(int keycode)override;
		virtual bool IsMouseButtonPressed(int mousecode)override;
		virtual double GetXpos()override;
		virtual double GetYpos()override;
		virtual std::pair<double, double> GetMousePos()override;
	};
	

	
}