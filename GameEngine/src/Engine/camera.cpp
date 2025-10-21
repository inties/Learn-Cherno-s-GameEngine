#include "pch.h"
#include "camera.h"
#include "Windows.h"
#include "Input.h"
namespace Engine {

    // 单例实例
    std::unique_ptr<MainCamera> MainCamera::s_Instance = nullptr;

    // 单例访问方法
    MainCamera* MainCamera::GetInstance() {
        return s_Instance.get();
    }

    // 初始化单例
    void MainCamera::Initialize(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
        MainCamera* rawPtr = new MainCamera(position, up, yaw, pitch);
        // 将原始指针交给unique_ptr管理
        s_Instance = std::unique_ptr<MainCamera>(rawPtr);
    }


  
    // 私有构造函数 - vectors
    MainCamera::MainCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :Camera(position, up), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        MouseLastX = Input::GetInstance()->GetXpos() / 2;
        MouseLastY = Input::GetInstance()->GetYpos() / 2;
        Yaw = yaw;
        Pitch = pitch;
        
        firstMouse = true;
        UpdateCameraVectors();
    }


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix


    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void MainCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void MainCamera::ProcessMouseMovement(float xPos, float yPos)
    {
        if (firstMouse)
        {
            MouseLastX = xPos;
            MouseLastY = yPos;
            firstMouse = false;
            return;
        }

		float xoffset = xPos - MouseLastX;
		float yoffset = MouseLastY - yPos; // reversed since y-coordinates go from bottom to top
		MouseLastX = xPos;
		MouseLastY = yPos;
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
      
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
        

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void MainCamera::ProcessMouseScroll(float yoffset)
    {
        fov_ -= (float)yoffset;
        if (fov_ < 1.0f)
            fov_ = 1.0f;
        if (fov_ > 45.0f)
            fov_ = 45.0f;
    }

    // calculates the front vector from the Camera's (updated) Euler Angles
    void MainCamera::UpdateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }

    Camera::Camera(glm::vec3 position, glm::vec3 up,glm::vec3 front, float near_value, float far_value, float aspect, float fov)
    {
        Position = position;
        WorldUp = up;
        Front = front;
        fov_ = fov;
        near_ = near_value;
        far_ = far_value;
        aspect_ = aspect;
        
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 Camera::GetProjectionMatrix() const {

        // 假设FOV, aspect, near, far

        return glm::perspective(glm::radians(fov_), aspect_, near_, far_); // 调整参数

    }


} // namespace Engine
