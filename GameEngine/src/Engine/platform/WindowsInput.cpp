#include"pch.h"
#include"Engine/platform/WindowsInput.h"
namespace Engine {
	WindowsInput::WindowsInput() {
		s_Instance = this;
	}
	bool WindowsInput::IsKeyPressed(int keycode) {
		GLFWwindow* window = glfwGetCurrentContext();
		int state = glfwGetKey(window, keycode);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}
	bool WindowsInput::IsMouseButtonPressed(int mousecode){
		GLFWwindow* window = glfwGetCurrentContext();
		int state = glfwGetMouseButton(window, mousecode);
		return (state == GLFW_PRESS);
	}
	float WindowsInput::GetXpos(){
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		return xpos;
	}
	float WindowsInput::GetYpos(){
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		return ypos;
	}
	std::pair<float, float> WindowsInput::GetMousePos(){
		double xpos, ypos;
		glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
		return { xpos, ypos };
	}


}