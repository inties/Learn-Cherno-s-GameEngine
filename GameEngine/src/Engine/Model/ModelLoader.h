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

// 用于存储Mesh数据的临时结构
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Ref<Material> material;
    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    Ref<Mesh> mesh;  // 创建后的Mesh引用
};

class ModelLoader {
public:
    static bool Load(const std::string& path, Ref<Model> model);

private:
    // 收集所有Mesh数据的新方法
    static void CollectMeshData(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath,
        std::vector<Vertex>& allVertices, std::vector<uint32_t>& allIndices, std::vector<MeshData>& meshDataList);
    
    // 创建共享VAO
    static Ref<VertexArray> CreateSharedVAO(const std::vector<Vertex>& allVertices, const std::vector<uint32_t>& allIndices);
    
    // 基于收集的数据创建Mesh
    static void CreateMeshesWithOffsets(std::vector<MeshData>& meshDataList, const Ref<VertexArray>& sharedVAO);

    // 修改后的ProcessNode（使用预创建的Mesh和局部索引计数器）
    static Ref<Node> ProcessNode(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath, 
        const std::vector<MeshData>& meshDataList, size_t& meshIndex);

    // 辅助方法
    static Ref<Material> ProcessMaterial(aiMaterial* assimpMaterial, const aiScene* scene, const std::string& modelPath, const std::string& directory);
    static Ref<Material> CreateDefaultMaterial(); // 创建默认材质
    static BufferLayout BuildVertexLayout(const std::vector<Vertex>& vertices); // 动态构建顶点布局
    static glm::mat4 ToGlm(const aiMatrix4x4& from);
    static std::string GetDirectory(const std::string& path);
    static std::string GetUniformName(aiTextureType type);
};

} 