// 示例：如何使用新的模型绘制系统
#include "pch.h"
// #include "Engine/Model/Model.h"
// #include "Engine/CameraManager.h"
// #include "Engine/camera.h"

// namespace Engine {

// void ModelExample() {
//     // 1. 设置全局相机
//     auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
//     CameraManager::SetGlobalCamera(std::move(camera));
    
//     // 2. 加载模型
//     auto model = Model::Create("assets/models/example.fbx");
//     if (!model) {
//         ENGINE_CORE_ERROR("Failed to load model");
//         return;
//     }
    
//     // 3. 设置模型变换
//     glm::mat4 modelTransform = glm::mat4(1.0f);
//     modelTransform = glm::translate(modelTransform, glm::vec3(0.0f, 0.0f, 0.0f));
//     modelTransform = glm::rotate(modelTransform, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//     model->SetGlobalTransform(modelTransform);
    
//     // 4. 在渲染循环中绘制
//     // Renderer::BeginScene(); // 如果需要
//     model->Draw();  // 内部会：
//                    // - 应用全局变换
//                    // - 递归遍历Node树
//                    // - 每个Mesh调用Draw(transform)
//                    // - Mesh::Draw调用Renderer::Submit
//                    // - Submit使用全局camera设置ViewProjection
//                    // - 使用共享VAO和偏移绘制
//     // Renderer::EndScene();
// }

// } // namespace Engine