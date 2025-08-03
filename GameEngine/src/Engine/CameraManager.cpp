#include "pch.h"
#include "CameraManager.h"

namespace Engine {

std::unique_ptr<Camera> CameraManager::s_GlobalCamera = nullptr;

void CameraManager::SetGlobalCamera(std::unique_ptr<Camera> camera) {
    s_GlobalCamera = std::move(camera);
}

Camera* CameraManager::GetGlobalCamera() {
    return s_GlobalCamera.get();
}

bool CameraManager::HasGlobalCamera() {
    return s_GlobalCamera != nullptr;
}

}