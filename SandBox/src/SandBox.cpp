#include "SandBox.h"
#include <iostream>

SandBox::SandBox()
{
    
}
//
SandBox::~SandBox()
{
   
}

//void SandBox::run()
//{
//    std::cout << "SandBox::run()" << std::endl;
//    
//    createSimpleWindow();
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
//    } else {
//        std::cout << "���ڴ���ʧ�ܣ�" << std::endl;
//    }
//    
//    std::cout << "SandBox::run() ����" << std::endl;
//}

//void SandBox::createSimpleWindow()
//{
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
//    
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
//}
//
//void SandBox::cleanup()
//{
//    if (m_Window) {
//        glfwDestroyWindow(m_Window);
//        m_Window = nullptr;
//    }
//    glfwTerminate();
//}

// ������ڵ�
Engine::Application* Engine::createApplication() 
{
    return new SandBox();
}
