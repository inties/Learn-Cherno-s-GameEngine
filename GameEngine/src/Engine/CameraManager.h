#pragma once

#include "Engine/core.h"
#include "Engine/camera.h"
#include <memory>

namespace Engine {

class CameraManager {
public:
    static void SetGlobalCamera(std::unique_ptr<Camera> camera);
    static Camera* GetGlobalCamera();
    static bool HasGlobalCamera();

private:
    static std::unique_ptr<Camera> s_GlobalCamera;
};

}