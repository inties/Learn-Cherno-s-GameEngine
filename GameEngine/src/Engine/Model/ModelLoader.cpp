#include "pch.h"

#include "ModelLoader.h"

#include "Engine/Resources/ShaderLibrary.h"

#include "Engine/Resources/TextureLibrary.h"

#include "Engine/Resources/MaterialLibrary.h"

#include "Engine/Resources/MeshLibrary.h"

#include "Engine/Renderer/Vertex.h"

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace Engine {

bool ModelLoader::Load(const std::string& path, Ref<Model> model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_SplitLargeMeshes | aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ENGINE_CORE_ERROR("Assimp error: {0}", importer.GetErrorString());
        return false;
    }

    // �ռ����ж������������
    std::vector<Vertex> allVertices;
    std::vector<uint32_t> allIndices;
    std::vector<MeshData> meshDataList;
    
    // �ݹ��ռ�����
    CollectMeshData(scene->mRootNode, scene, path, allVertices, allIndices, meshDataList);
    
    // ��֤�ռ�������
    if (allVertices.empty() || allIndices.empty() || meshDataList.empty()) {
        ENGINE_CORE_ERROR("No valid mesh data found in model: {}", path);
        return false;
    }
    
    // ��������VAO���ϴ�����
    model->m_SharedVAO = CreateSharedVAO(allVertices, allIndices);
    if (!model->m_SharedVAO) {
        ENGINE_CORE_ERROR("Failed to create shared VAO for model: {}", path);
        return false;
    }
    
    // ����Mesh��������ƫ��
    CreateMeshesWithOffsets(meshDataList, model->m_SharedVAO);
    
    // ��������ͼ��ʹ�þֲ�����������
    size_t meshIndex = 0;
    model->m_RootNode = ProcessNode(scene->mRootNode, scene, path, meshDataList, meshIndex);

    return true;
}

Ref<Node> ModelLoader::ProcessNode(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath, 
    const std::vector<MeshData>& meshDataList, size_t& meshIndex) {
    
    Ref<Node> node = CreateRef<Node>();
    node->m_Name = assimpNode->mName.C_Str();
    node->m_LocalTransform = ToGlm(assimpNode->mTransformation);

    // ���Ҳ���Ӷ�Ӧ��Mesh��ͨ������ӳ�䣩
    for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
        if (meshIndex < meshDataList.size()) {
            node->m_Meshes.push_back(meshDataList[meshIndex].mesh);
            meshIndex++;
        }
    }

    // �ݹ鴦���ӽڵ�
    for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
        node->m_Children.push_back(ProcessNode(assimpNode->mChildren[i], scene, modelPath, meshDataList, meshIndex));
    }

    return node;
}

// ProcessMesh�������Ƴ���ʹ���µĹ����������

Ref<Material> ModelLoader::ProcessMaterial(aiMaterial* assimpMaterial, const aiScene* scene, const std::string& modelPath, const std::string& directory) {
    if (!assimpMaterial) {
        ENGINE_CORE_ERROR("Invalid aiMaterial pointer in ProcessMaterial");
        return nullptr;
    }

    // ��ȡ�򴴽�Ĭ����ɫ��
    Ref<Shader> shader = ShaderLibrary::Get()->Get("DefaultPBR");
    if (!shader) {
        ENGINE_CORE_WARN("DefaultPBR shader not found, attempting to load default shader");
        // ���Լ���Ĭ����ɫ�������ʧ���򴴽����������ɫ��
        shader = ShaderLibrary::Get()->Load("DefaultPBR", "assets/shaders/default.glsl");
        if (!shader) {
            ENGINE_CORE_ERROR("Failed to load default shader, material creation failed");
            return nullptr;
        }
    }

    Ref<Material> material = Material::Create(shader);
    if (!material) {
        ENGINE_CORE_ERROR("Failed to create material");
        return nullptr;
    }

    // ����������������
    for (int t = aiTextureType_DIFFUSE; t <= aiTextureType_HEIGHT; ++t) {
        aiTextureType type = static_cast<aiTextureType>(t);
        unsigned int textureCount = assimpMaterial->GetTextureCount(type);
        
        for (unsigned int i = 0; i < textureCount; ++i) {
            aiString texPath;
            if (assimpMaterial->GetTexture(type, i, &texPath) != AI_SUCCESS) {
                ENGINE_CORE_WARN("Failed to get texture path for type {} index {} in material", static_cast<int>(type), i);
                continue;
            }
            
            if (texPath.length == 0) {
                ENGINE_CORE_WARN("Empty texture path for type {} index {}", static_cast<int>(type), i);
                continue;
            }
            
            std::string fullPath = directory + "/" + texPath.C_Str();
            
            try {
                Ref<Texture2D> texture = TextureLibrary::Get()->Load(fullPath);
                if (texture) {
                    std::string uniform = GetUniformName(type);
                    if (i > 0) uniform += std::to_string(i); // ֻ�ж������ʱ���������
                    material->SetTexture(uniform, texture);
                    ENGINE_CORE_INFO("Loaded texture: {} as uniform: {}", fullPath, uniform);
                } else {
                    ENGINE_CORE_WARN("Failed to load texture: {}", fullPath);
                }
            } catch (const std::exception& e) {
                ENGINE_CORE_ERROR("Exception loading texture {}: {}", fullPath, e.what());
            }
        }
    }

    // ������������

    aiColor4D color;

    if (aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {

        material->SetFloat4("u_DiffuseColor", glm::vec4(color.r, color.g, color.b, color.a));

    }

    // TODO: �������������ȡ��ֲڶȵ�

    return material;

}

glm::mat4 ModelLoader::ToGlm(const aiMatrix4x4& from) {

    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;

    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;

    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;

    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}

void ModelLoader::CollectMeshData(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath,
    std::vector<Vertex>& allVertices, std::vector<uint32_t>& allIndices, std::vector<MeshData>& meshDataList) {
    
    if (!assimpNode || !scene) {
        ENGINE_CORE_ERROR("Invalid aiNode or aiScene pointer in CollectMeshData");
        return;
    }
    
    // ����ǰ�ڵ������Mesh
    for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
        unsigned int meshIndex = assimpNode->mMeshes[i];
        if (meshIndex >= scene->mNumMeshes) {
            ENGINE_CORE_ERROR("Invalid mesh index {} in node (max: {})", meshIndex, scene->mNumMeshes);
            continue;
        }
        
        aiMesh* assimpMesh = scene->mMeshes[meshIndex];
        if (!assimpMesh) {
            ENGINE_CORE_ERROR("Null aiMesh at index {}", meshIndex);
            continue;
        }
        
        if (assimpMesh->mNumVertices == 0) {
            ENGINE_CORE_WARN("Mesh {} has no vertices, skipping", meshIndex);
            continue;
        }
        
        MeshData meshData;
        // ��ȡ��������
        for (unsigned int v = 0; v < assimpMesh->mNumVertices; ++v) {
            Vertex vertex;
            vertex.Position = glm::vec3(assimpMesh->mVertices[v].x, assimpMesh->mVertices[v].y, assimpMesh->mVertices[v].z);
            if (assimpMesh->HasNormals()) 
                vertex.Normal = glm::vec3(assimpMesh->mNormals[v].x, assimpMesh->mNormals[v].y, assimpMesh->mNormals[v].z);
            if (assimpMesh->mTextureCoords[0]) 
                vertex.TexCoord = glm::vec2(assimpMesh->mTextureCoords[0][v].x, assimpMesh->mTextureCoords[0][v].y);
            if (assimpMesh->HasTangentsAndBitangents()) {
                vertex.Tangent = glm::vec3(assimpMesh->mTangents[v].x, assimpMesh->mTangents[v].y, assimpMesh->mTangents[v].z);
                vertex.Bitangent = glm::vec3(assimpMesh->mBitangents[v].x, assimpMesh->mBitangents[v].y, assimpMesh->mBitangents[v].z);
            }
            meshData.vertices.push_back(vertex);
        }
        
        // ��ȡ��������
        for (unsigned int f = 0; f < assimpMesh->mNumFaces; ++f) {
            aiFace face = assimpMesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                meshData.indices.push_back(face.mIndices[j]);
            }
        }
        
        // ������ʣ��������飩
        unsigned int matIndex = assimpMesh->mMaterialIndex;
        if (matIndex >= scene->mNumMaterials) {
            ENGINE_CORE_ERROR("Invalid material index {} for mesh (max: {})", matIndex, scene->mNumMaterials);
            // ʹ��Ĭ�ϲ���
            meshData.material = CreateDefaultMaterial();
        } else {
            std::string matKey = modelPath + "::mat::" + std::to_string(matIndex);
            if (MaterialLibrary::Get()->Exists(matKey)) {
                meshData.material = MaterialLibrary::Get()->Get(matKey);
            } else {
                meshData.material = ProcessMaterial(scene->mMaterials[matIndex], scene, modelPath, GetDirectory(modelPath));
                if (meshData.material) {
                    MaterialLibrary::Get()->Add(matKey, meshData.material);
                } else {
                    ENGINE_CORE_ERROR("Failed to process material {}, using default", matIndex);
                    meshData.material = CreateDefaultMaterial();
                }
            }
        }
        
        if (!meshData.material) {
            ENGINE_CORE_ERROR("Failed to create/load material for mesh, skipping");
            continue;
        }
        
        // ����ƫ��
        meshData.vertexOffset = allVertices.size();
        meshData.indexOffset = allIndices.size();
        
        // ��ӵ�ȫ������
        allVertices.insert(allVertices.end(), meshData.vertices.begin(), meshData.vertices.end());
        allIndices.insert(allIndices.end(), meshData.indices.begin(), meshData.indices.end());
        
        meshDataList.push_back(meshData);
    }
    
    // �ݹ鴦���ӽڵ�
    for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
        CollectMeshData(assimpNode->mChildren[i], scene, modelPath, allVertices, allIndices, meshDataList);
    }
}

Ref<VertexArray> ModelLoader::CreateSharedVAO(const std::vector<Vertex>& allVertices, const std::vector<uint32_t>& allIndices) {
    if (allVertices.empty()) {
        ENGINE_CORE_ERROR("Cannot create VAO with empty vertex data");
        return nullptr;
    }
    
    if (allIndices.empty()) {
        ENGINE_CORE_ERROR("Cannot create VAO with empty index data");
        return nullptr;
    }
    
    auto vao = VertexArray::Create();
    if (!vao) {
        ENGINE_CORE_ERROR("Failed to create VertexArray");
        return nullptr;
    }
    
    try {
        // �������ϴ����㻺����
        auto vbo = VertexBuffer::Create(reinterpret_cast<float*>(const_cast<Vertex*>(allVertices.data())), 
            allVertices.size() * sizeof(Vertex));
        if (!vbo) {
            ENGINE_CORE_ERROR("Failed to create VertexBuffer");
            return nullptr;
        }
        
        // ��̬�������֣�����ʵ�ʶ������ݣ�
        BufferLayout layout = BuildVertexLayout(allVertices);
        vbo->SetLayout(layout);
        vao->SetVertexBuffer(vbo);
        
        // �������ϴ�����������
        auto ibo = IndexBuffer::Create(const_cast<uint32_t*>(allIndices.data()), allIndices.size());
        if (!ibo) {
            ENGINE_CORE_ERROR("Failed to create IndexBuffer");
            return nullptr;
        }
        vao->SetIndexBuffer(ibo);
        
        ENGINE_CORE_INFO("Successfully created shared VAO with {} vertices and {} indices", 
            allVertices.size(), allIndices.size());
        
    } catch (const std::exception& e) {
        ENGINE_CORE_ERROR("Exception creating shared VAO: {}", e.what());
        return nullptr;
    }
    
    return vao;
}

void ModelLoader::CreateMeshesWithOffsets(std::vector<MeshData>& meshDataList, const Ref<VertexArray>& sharedVAO) {
    if (!sharedVAO) {
        ENGINE_CORE_ERROR("Cannot create meshes with null shared VAO");
        return;
    }
    
    for (auto& meshData : meshDataList) {
        if (!meshData.material) {
            ENGINE_CORE_ERROR("Cannot create mesh with null material, skipping");
            continue;
        }
        
        try {
            meshData.mesh = CreateRef<Mesh>(meshData.material, sharedVAO, 
                meshData.vertexOffset, meshData.vertices.size(),
                meshData.indexOffset, meshData.indices.size());
                
            if (!meshData.mesh) {
                ENGINE_CORE_ERROR("Failed to create mesh");
            }
        } catch (const std::exception& e) {
            ENGINE_CORE_ERROR("Exception creating mesh: {}", e.what());
        }
    }
}

std::string ModelLoader::GetDirectory(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

Ref<Material> ModelLoader::CreateDefaultMaterial() {
    // ���Ի�ȡ�򴴽����������ɫ��
    Ref<Shader> defaultShader = ShaderLibrary::Get()->Get("Default");
    if (!defaultShader) {
        ENGINE_CORE_WARN("No default shader available, attempting to create basic shader");
        // ������Դ���һ�����������ɫ���򷵻�nullptr
        // ʵ��ʵ���п�����ҪӲ����һ����򵥵���ɫ��
        return nullptr;
    }
    
    auto material = Material::Create(defaultShader);
    if (material) {
        // ����Ĭ����ɫ
        material->SetFloat4("u_DiffuseColor", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        ENGINE_CORE_INFO("Created default material");
    }
    
    return material;
}

BufferLayout ModelLoader::BuildVertexLayout(const std::vector<Vertex>& vertices) {
    // ʼ�շ���������5�����Բ��֣�ȷ��stride��Vertex�ṹ���Сһ��
    // �������Ա��ⶥ�����ݶ�ȡʱ�Ĳ�����������
    ENGINE_CORE_INFO("Building fixed vertex layout with 5 attributes (stride = {})", sizeof(Vertex));
    
    return {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float3, "a_Normal"},
        {ShaderDataType::Float2, "a_TexCoord"},
        {ShaderDataType::Float3, "a_Tangent"},
        {ShaderDataType::Float3, "a_Bitangent"}
    };
}

std::string ModelLoader::GetUniformName(aiTextureType type) {
    switch (type) {
        case aiTextureType_DIFFUSE: return "u_AlbedoMap";
        case aiTextureType_SPECULAR: return "u_SpecularMap";
        case aiTextureType_HEIGHT: return "u_NormalMap";
        case aiTextureType_NORMALS: return "u_NormalMap";
        case aiTextureType_METALNESS: return "u_MetallicMap";
        case aiTextureType_DIFFUSE_ROUGHNESS: return "u_RoughnessMap";
        case aiTextureType_AMBIENT_OCCLUSION: return "u_AOMap";
        // TODO: ��������
        default: return "u_Texture";
    }
}

} 