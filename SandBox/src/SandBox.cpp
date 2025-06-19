#include "SandBox.h"
#include<Engine.h>
#include <iostream>
#include <glad/glad.h>
SandBox::SandBox()
{
	Engine::Windows::WindowsProps props = { "SandBox", 800, 600 };
	m_Window = std::unique_ptr<Engine::Windows>(Engine::Windows::Create(props));
	if (m_Window == nullptr)
	{
		std::cout << "Window creation failed!" << std::endl;
		return;
	}	
}
//
SandBox::~SandBox()
{
   
}
//void SandBox::run() {
//    std::cout << "SandBox::run()" << std::endl;
//
//    std::cout << "��ʼ��GLFW..." << std::endl;
//
//    if (!glfwInit()) {
//        std::cout << "GLFW��ʼ��ʧ��!" << std::endl;
//        return;
//    }
//
//    // ����GLFW
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    GLFWwindow* m_Window = nullptr;
//    // ��������
//    m_Window = glfwCreateWindow(800, 600, "SandBox - GameEngine", nullptr, nullptr);
//    if (!m_Window) {
//        std::cout << "GLFW���ڴ���ʧ��!" << std::endl;
//        glfwTerminate();
//        return;
//    }
//
//    glfwMakeContextCurrent(m_Window);
//
//    // ��ʼ��GLAD
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cout << "GLAD��ʼ��ʧ��!" << std::endl;
//        return;
//    }
//
//    std::cout << "OpenGL��Ϣ:" << std::endl;
//    std::cout << "�汾: " << glGetString(GL_VERSION) << std::endl;
//    std::cout << "����: " << glGetString(GL_VENDOR) << std::endl;
//
//    glViewport(0, 0, 800, 600);
//
//    if (m_Window) {
//        std::cout << "Window Created!" << std::endl;
//
//        // �򵥵���ѭ��
//        while (!glfwWindowShouldClose(m_Window))
//        {
//            // �����¼�
//            glfwPollEvents();
//
//            // ���ESC��
//            if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//                glfwSetWindowShouldClose(m_Window, true);
//            }
//
//            // ����Ⱦ
//            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT);
//
//            glfwSwapBuffers(m_Window);
//        }
//    }
//    else {
//        std::cout << "���ڴ���ʧ�ܣ�" << std::endl;
//    }
//
//    std::cout << "SandBox::run() ����" << std::endl;
//}
void SandBox::run() {
	m_Window->Update();
    //���ô��ڵ�undate����
}
// ������ڵ�
Engine::Application* Engine::createApplication() 
{
    return new SandBox();
}
