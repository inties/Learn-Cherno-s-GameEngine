#ifndef CAMERA_H
#define CAMERA_H

#include "pch.h"

namespace Engine {

    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 10.0f;
    const float SENSITIVITY = 0.4f;
    const float ZOOM = 45.0f;
    class Camera {
  
    public:
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3 front=glm::vec3(0.0f,0.0f,-1.0f),
            float near_value=0.1f,float far_value=1000.0f,float aspect=1.0f,float fov=45.0f  
            );
        ~Camera() = default;
        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix() const;

        inline void SetFront(const glm::vec3& front) { Front = front; }
        inline void SetUp(const glm::vec3& up) { Up = up; }
        inline void SetPosition(const glm::vec3& position) { Position = position; }
        inline void SetFOV(float fov) { fov_ = fov; }
        inline void SetNear(float near_value) { near_ = near_value; }
        inline void SetFar(float far_value) { far_ = far_value; }
        inline void SetAspect(float aspect) { aspect_ = aspect; }
    protected:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
 
        // camera options
        float fov_;
        float near_;
        float far_;
        float aspect_;
    };
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class MainCamera:public Camera
    {
    public:
        static MainCamera* GetInstance();
        static void Initialize(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
                              glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
                              float yaw = YAW, float pitch = PITCH);
		inline void SetAspectRatio(float aspect) { aspect_ = aspect; }
		inline void SetNearPlane(float nearPlane) { near_ = nearPlane; }
		inline void SetFarPlane(float farPlane) { far_ = farPlane; }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix

  
        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xPos, float yPos);
        
        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset);
        

        void SetPosition(const glm::vec3& position) { Position = position; }
        const glm::vec3& GetPosition() const { return Position; }
        const glm::vec3& GetFront() const { return Front; }
        inline bool& isFirstMouse() { return firstMouse; };
    private:

        MainCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        MainCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
        

        static std::unique_ptr<MainCamera> s_Instance;

        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
        float MouseLastX;
        float MouseLastY;
        bool firstMouse;
        float MovementSpeed;
        float MouseSensitivity;
        // euler Angles
        float Yaw;
        float Pitch;
    };

} // namespace Engine

#endif

