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
		
		// ?????????
		if (!glfwInit()) {
			ENGINE_CORE_ERROR("glfw Failed to Init");
			return;
		}
		// ????GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		// ????????
		m_Window = glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
		if (!m_Window) {
			std::cout << "GLFW??????????!" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);

		// ?????GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "GLAD????????!" << std::endl;
			return;
		}

		std::cout << "OpenGL???:" << std::endl;
		std::cout << "?��: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "????: " << glGetString(GL_VENDOR) << std::endl;

		glViewport(0, 0, m_Props.Width, m_Props.Height);

		if (m_Window) {
			std::cout << "Window Created!" << std::endl;

			
		}
		else {
			std::cout << "???????????" << std::endl;
		}

		//---------------------------???glfw??????---------------------------
		glfwSetWindowUserPointer(m_Window, &m_Props); // ?????????????????????
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

		// ???????????????
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				
				MouseMoveEvent event(xpos, ypos);
				data.EventCallback(event);
			});
		// ??? framebuffer size callback ????????????????????��
		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				// ????????????��??????ImGui???
				int logicalWidth, logicalHeight;
				glfwGetWindowSize(window, &logicalWidth, &logicalHeight);
				data.Width = logicalWidth;
				data.Height = logicalHeight;

				// ?????????????��???????????OpenGL??????????
				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		// ????????????
		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int count, const char** paths)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				
				// ??C?????????????std::vector<std::string>
				std::vector<std::string> filePaths;
				for (int i = 0; i < count; ++i) {
					filePaths.push_back(std::string(paths[i]));
				}
				
				// ???????????????????
				FileDragDropEvent event(filePaths);
				data.EventCallback(event);
			});

		//-----------------------????Input??
		
	}
	WindowWindows::~WindowWindows()
	{
		// ???????
	}
	void WindowWindows::Update()
	{	
		
			// ???????
			glfwPollEvents();
			// ?????
		

			glfwSwapBuffers(m_Window);
	}
	void WindowWindows::SetEventCallBack(const EventFuc& callback)
	{
		m_Props.EventCallback = callback;
	}
	//???glfw???????????
}