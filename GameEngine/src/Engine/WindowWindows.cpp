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

		std::cout << "SandBox::run() ����" << std::endl;
	}
	WindowWindows::~WindowWindows()
	{
		// ������Դ
	}
	void WindowWindows::Update()
	{
		// �򵥵���ѭ��
		while (!glfwWindowShouldClose(m_Window))
		{
			// �����¼�
			glfwPollEvents();

			// ���ESC��
			if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(m_Window, true);
			}

			// ����Ⱦ
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glfwSwapBuffers(m_Window);
		}
	}
	void WindowWindows::SetEventCallBack(const EventFuc& callback)
	{
		m_Props.EventCallback = callback;
	}
	//ʹ��glfwע�ᴰ���¼��ص�
}