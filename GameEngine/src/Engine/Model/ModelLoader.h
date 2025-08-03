#pragma once

#include "Engine/core.h"
#include "Engine/Model/Model.h"
#include "Engine/Renderer/Vertex.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Buffer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

namespace Engine {

// ���ڴ洢Mesh���ݵ���ʱ�ṹ
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Ref<Material> material;
    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    Ref<Mesh> mesh;  // �������Mesh����
};

class ModelLoader {
public:
    static bool Load(const std::string& path, Ref<Model> model);

private:
    // �ռ�����Mesh���ݵ��·���
    static void CollectMeshData(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath,
        std::vector<Vertex>& allVertices, std::vector<uint32_t>& allIndices, std::vector<MeshData>& meshDataList);
    
    // ��������VAO
    static Ref<VertexArray> CreateSharedVAO(const std::vector<Vertex>& allVertices, const std::vector<uint32_t>& allIndices);
    
    // �����ռ������ݴ���Mesh
    static void CreateMeshesWithOffsets(std::vector<MeshData>& meshDataList, const Ref<VertexArray>& sharedVAO);

    // �޸ĺ��ProcessNode��ʹ��Ԥ������Mesh�;ֲ�������������
    static Ref<Node> ProcessNode(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath, 
        const std::vector<MeshData>& meshDataList, size_t& meshIndex);

    // ��������
    static Ref<Material> ProcessMaterial(aiMaterial* assimpMaterial, const aiScene* scene, const std::string& modelPath, const std::string& directory);
    static Ref<Material> CreateDefaultMaterial(); // ����Ĭ�ϲ���
    static BufferLayout BuildVertexLayout(const std::vector<Vertex>& vertices); // ��̬�������㲼��
    static glm::mat4 ToGlm(const aiMatrix4x4& from);
    static std::string GetDirectory(const std::string& path);
    static std::string GetUniformName(aiTextureType type);
};

} 