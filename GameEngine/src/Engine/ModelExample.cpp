// ʾ�������ʹ���µ�ģ�ͻ���ϵͳ
#include "pch.h"
// #include "Engine/Model/Model.h"
// #include "Engine/CameraManager.h"
// #include "Engine/camera.h"

// namespace Engine {

// void ModelExample() {
//     // 1. ����ȫ�����
//     auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
//     CameraManager::SetGlobalCamera(std::move(camera));
    
//     // 2. ����ģ��
//     auto model = Model::Create("assets/models/example.fbx");
//     if (!model) {
//         ENGINE_CORE_ERROR("Failed to load model");
//         return;
//     }
    
//     // 3. ����ģ�ͱ任
//     glm::mat4 modelTransform = glm::mat4(1.0f);
//     modelTransform = glm::translate(modelTransform, glm::vec3(0.0f, 0.0f, 0.0f));
//     modelTransform = glm::rotate(modelTransform, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//     model->SetGlobalTransform(modelTransform);
    
//     // 4. ����Ⱦѭ���л���
//     // Renderer::BeginScene(); // �����Ҫ
//     model->Draw();  // �ڲ��᣺
//                    // - Ӧ��ȫ�ֱ任
//                    // - �ݹ����Node��
//                    // - ÿ��Mesh����Draw(transform)
//                    // - Mesh::Draw����Renderer::Submit
//                    // - Submitʹ��ȫ��camera����ViewProjection
//                    // - ʹ�ù���VAO��ƫ�ƻ���
//     // Renderer::EndScene();
// }

// } // namespace Engine