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
		
		// ��ʼ������
		if (!glfwInit()) {
			ENGINE_CORE_ERROR("glfw Failed to Init");
			return;
		}
		// ����GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		// ��������
		m_Window = glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
		if (!m_Window) {
			std::cout << "GLFW���ڴ���ʧ��!" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);

		// ��ʼ��GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "GLAD��ʼ��ʧ��!" << std::endl;
			return;
		}

		std::cout << "OpenGL��Ϣ:" << std::endl;
		std::cout << "�汾: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "����: " << glGetString(GL_VENDOR) << std::endl;

		glViewport(0, 0, m_Props.Width, m_Props.Height);

		if (m_Window) {
			std::cout << "Window Created!" << std::endl;

			
		}
		else {
			std::cout << "���ڴ���ʧ�ܣ�" << std::endl;
		}

		//---------------------------ע��glfw�¼��ص�---------------------------
		glfwSetWindowUserPointer(m_Window, &m_Props); // �����û�ָ��Ϊ��ǰ���ڶ���
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

		// ע������ƶ��¼��ص�
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				
				MouseMoveEvent event(xpos, ypos);
				data.EventCallback(event);
			});
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowsProps& data = *(WindowsProps*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

	}
	WindowWindows::~WindowWindows()
	{
		// ������Դ
	}
	void WindowWindows::Update()
	{	
			// �����¼�
			glfwPollEvents();
			// ����Ⱦ
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glfwSwapBuffers(m_Window);
	}
	void WindowWindows::SetEventCallBack(const EventFuc& callback)
	{
		m_Props.EventCallback = callback;
	}
	//ʹ��glfwע�ᴰ���¼��ص�
}