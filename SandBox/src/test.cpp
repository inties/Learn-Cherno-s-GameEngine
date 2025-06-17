// #include <Engine.h>  
// #include <iostream>
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// class SandBox : public Engine::Application  
// {  
// private:
//     GLFWwindow* window;
    
// public:  
//     SandBox() {
//         std::cout << "初始化SandBox应用程序..." << std::endl;
//         initializeGLFWAndGlad();
//     }  

//     ~SandBox() {
//         cleanup();
//     }

// private:
//     void initializeGLFWAndGlad() {
//         // 初始化GLFW
//         if (!glfwInit()) {
//             std::cout << "错误: 无法初始化GLFW" << std::endl;
//             return;
//         }

//         // 设置GLFW窗口属性
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//         // 创建窗口
//         window = glfwCreateWindow(800, 600, "Glad演示 - GameEngine", NULL, NULL);
//         if (!window) {
//             std::cout << "错误: 无法创建GLFW窗口" << std::endl;
//             glfwTerminate();
//             return;
//         }

//         // 设置当前上下文
//         glfwMakeContextCurrent(window);

//         // 初始化GLAD
//         if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//             std::cout << "错误: 无法初始化GLAD" << std::endl;
//             return;
//         }

//         // 输出OpenGL信息
//         std::cout << "==================== OpenGL信息 ====================" << std::endl;
//         std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
//         std::cout << "OpenGL供应商: " << glGetString(GL_VENDOR) << std::endl;
//         std::cout << "OpenGL渲染器: " << glGetString(GL_RENDERER) << std::endl;
//         std::cout << "GLSL版本: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
//         std::cout << "=================================================" << std::endl;

//         // 设置视口
//         glViewport(0, 0, 800, 600);

//         // 运行简单的演示
//         runGladDemo();
//     }

//     void runGladDemo() {
//         // 设置背景颜色
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

//         std::cout << "Glad演示开始 - 按ESC键退出" << std::endl;
        
//         // 主循环
//         while (!glfwWindowShouldClose(window)) {
//             // 处理输入
//             processInput();

//             // 渲染
//             render();

//             // 交换缓冲区并轮询事件
//             glfwSwapBuffers(window);
//             glfwPollEvents();
//         }
//     }

//     void processInput() {
//         if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//             glfwSetWindowShouldClose(window, true);
//         }
//     }

//     void render() {
//         // 清除颜色缓冲区
//         glClear(GL_COLOR_BUFFER_BIT);

//         // 绘制一个彩色三角形（使用现代OpenGL）
//         drawTriangle();
//     }

//     void drawTriangle() {
//         // 使用OpenGL立即模式绘制三角形（适合快速演示）
//         glBegin(GL_TRIANGLES);
        
//         // 顶点1 - 红色
//         glColor3f(1.0f, 0.0f, 0.0f);
//         glVertex2f(0.0f, 0.5f);
        
//         // 顶点2 - 绿色
//         glColor3f(0.0f, 1.0f, 0.0f);
//         glVertex2f(-0.5f, -0.5f);
        
//         // 顶点3 - 蓝色
//         glColor3f(0.0f, 0.0f, 1.0f);
//         glVertex2f(0.5f, -0.5f);
        
//         glEnd();
//     }

//     void cleanup() {
//         if (window) {
//             glfwDestroyWindow(window);
//         }
//         glfwTerminate();
//         std::cout << "SandBox应用程序已清理完毕" << std::endl;
//     }
// };  

// Engine::Application* Engine::createApplication() {  
//     return new SandBox();  
// }
