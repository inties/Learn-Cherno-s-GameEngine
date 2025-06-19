#include"pch.h"
#include"log.h"
#include"Event.h"
#include"WindowWindows.h"

namespace Engine {
	WindowWindows::WindowWindows(WindowsProps& props)
	{
		m_Props.EventCallback = props.EventCallback;
		m_Props.Height = props.Height;
		m_Props.Width = props.Width;
		m_Props.Title = props.Title;
		ENGINE_CORE_INFO("WindowsWindow created with {0}({1}{2})", m_Props.Title, m_Props.Width, m_Props.Height);
		
		// 初始化窗口
		if (!glfwInit()) {
			ENGINE_CORE_ERROR("glfw Failed to Init");
			return;
		}
		// 设置GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		// 创建窗口
		m_Window = glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
		if (!m_Window) {
			std::cout << "GLFW窗口创建失败!" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);

		// 初始化GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "GLAD初始化失败!" << std::endl;
			return;
		}

		std::cout << "OpenGL信息:" << std::endl;
		std::cout << "版本: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "厂商: " << glGetString(GL_VENDOR) << std::endl;

		glViewport(0, 0, m_Props.Width, m_Props.Height);

		if (m_Window) {
			std::cout << "Window Created!" << std::endl;

			
		}
		else {
			std::cout << "窗口创建失败！" << std::endl;
		}

		std::cout << "SandBox::run() 结束" << std::endl;
	}
	WindowWindows::~WindowWindows()
	{
		// 清理资源
	}
	void WindowWindows::Update()
	{
		// 简单的主循环
		while (!glfwWindowShouldClose(m_Window))
		{
			// 处理事件
			glfwPollEvents();

			// 检查ESC键
			if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(m_Window, true);
			}

			// 简单渲染
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glfwSwapBuffers(m_Window);
		}
	}
	void WindowWindows::SetEventCallBack(const EventFuc& callback)
	{
		m_Props.EventCallback = callback;
	}
	//使用glfw注册窗口事件回调
}