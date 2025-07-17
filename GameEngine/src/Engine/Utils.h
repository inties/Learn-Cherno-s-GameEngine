//#pragma once
//
//#include <GLFW/glfw3.h>
//#include <camera.h>
//#include<string>
//#include <iostream>
//extern float lastX;
//extern float lastY;
//extern bool firstMouse;
//extern Camera camera;
//extern float deltaTime;
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
//{
//    return a + f * (b - a);
//}
//GLenum glCheckError_(const char* file, int line)
//{
//    GLenum errorCode;
//    while ((errorCode = glGetError()) != GL_NO_ERROR)
//    {
//        std::string error;
//        switch (errorCode)
//        {
//        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
//        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
//        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
//        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
//        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
//        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
//        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
//        }
//        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
//    }
//    return errorCode;
//}
//
//#define glCheckError() glCheckError_(__FILE__, __LINE__) 
//void hello() {}
//#define HELLO 1
