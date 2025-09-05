#include"pch.h"
#include"Engine/log.h"
#include"Engine/Event.h"
#include"Engine/platform/WindowWindows.h"
#include"Engine/platform/WindowsInput.h"

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

		//---------------------------注册glfw事件回调---------------------------
		glfwSetWindowUserPointer(m_Window, &m_Props); // 设置用户指针为当前窗口对象
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		// 注册鼠标移动事件回调
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				
				MouseMoveEvent event(xpos, ypos);
				data.EventCallback(event);
			});
		// 使用 framebuffer size callback 来获取真正的物理像素大小
		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				// 更新逻辑窗口大小（用于ImGui等）
				int logicalWidth, logicalHeight;
				glfwGetWindowSize(window, &logicalWidth, &logicalHeight);
				data.Width = logicalWidth;
				data.Height = logicalHeight;

				// 发送物理像素大小的事件（用于OpenGL视口和投影矩阵）
				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		// 注册文件拖拽回调
		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int count, const char** paths)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				
				// 将C字符串数组转换为std::vector<std::string>
				std::vector<std::string> filePaths;
				for (int i = 0; i < count; ++i) {
					filePaths.push_back(std::string(paths[i]));
				}
				
				// 创建并发送文件拖拽事件
				FileDragDropEvent event(filePaths);
				data.EventCallback(event);
			});

		//-----------------------创建Input类
		
	}
	WindowWindows::~WindowWindows()
	{
		// 清理资源
	}
	void WindowWindows::Update()
	{	
		
			// 处理事件
			glfwPollEvents();
			// 简单渲染
		

			glfwSwapBuffers(m_Window);
	}
	void WindowWindows::SetEventCallBack(const EventFuc& callback)
	{
		m_Props.EventCallback = callback;
	}
	//使用glfw注册窗口事件回调
}