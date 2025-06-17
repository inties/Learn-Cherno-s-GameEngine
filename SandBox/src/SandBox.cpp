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
//        // 简单的主循环
//        while (!glfwWindowShouldClose(m_Window))
//        {
//            // 处理事件
//            glfwPollEvents();
//            
//            // 检查ESC键
//            if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//                glfwSetWindowShouldClose(m_Window, true);
//            }
//            
//            // 简单渲染
//            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT);
//            
//            glfwSwapBuffers(m_Window);
//        }
//    } else {
//        std::cout << "窗口创建失败！" << std::endl;
//    }
//    
//    std::cout << "SandBox::run() 结束" << std::endl;
//}

//void SandBox::createSimpleWindow()
//{
//    std::cout << "初始化GLFW..." << std::endl;
//    
//    if (!glfwInit()) {
//        std::cout << "GLFW初始化失败!" << std::endl;
//        return;
//    }
//    
//    // 设置GLFW
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    
//    // 创建窗口
//    m_Window = glfwCreateWindow(800, 600, "SandBox - GameEngine", nullptr, nullptr);
//    if (!m_Window) {
//        std::cout << "GLFW窗口创建失败!" << std::endl;
//        glfwTerminate();
//        return;
//    }
//    
//    glfwMakeContextCurrent(m_Window);
//    
//    // 初始化GLAD
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cout << "GLAD初始化失败!" << std::endl;
//        return;
//    }
//    
//    std::cout << "OpenGL信息:" << std::endl;
//    std::cout << "版本: " << glGetString(GL_VERSION) << std::endl;
//    std::cout << "厂商: " << glGetString(GL_VENDOR) << std::endl;
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

// 引擎入口点
Engine::Application* Engine::createApplication() 
{
    return new SandBox();
}
