将 `Mesh` 和 `Material` 纳入资源管理系统，其核心思想与 `TextureLibrary` 完全一致：**为每一个独立的资源赋予一个唯一的身份标识（Key），并建立一个中央缓存库（Library/Cache），实现“一次加载，处处复用”。**

下面我们来分步探讨如何实现。

### 第一步：核心思路与原则

1.  **唯一标识 (Unique Identifier)**：如何给一个 Mesh 或 Material 定义一个不会重复的“身份证号”？

      * 对于 **Texture**，文件路径 `path/to/texture.png` 是天然的唯一标识。
      * 对于 **Mesh** 和 **Material**，它们通常不是独立的文件，而是被包含在模型文件（如 `.fbx`, `.gltf`）内部。因此，它们的唯一标识需要是**复合**的，例如：`模型文件路径 + 网格在文件中的索引/名称`。

2.  **中央缓存库 (Central Cache)**：我们需要创建 `MeshLibrary` 和 `MaterialLibrary`，像 `TextureLibrary` 一样，通常实现为单例或在引擎上下文中统一管理。它们内部会有一个 `std::unordered_map`，将唯一标识映射到加载好的资源上。

3.  **生命周期管理 (Lifetime Management)**：继续使用 `Ref<>` (智能指针，如 `std::shared_ptr`) 来管理资源。当没有任何对象（如 `Model` 实例或 `Node`）引用某个 `Mesh` 或 `Material` 时，它的引用计数归零，资源被自动销毁。

### 第二步：设计资源库与唯一标识

#### 1\. 定义资源库

我们可以仿照 `TextureLibrary` 创建 `MeshLibrary` 和 `MaterialLibrary`。

```cpp
// MeshLibrary.h
class MeshLibrary {
public:
    static Ref<MeshLibrary> Get(); // 获取单例

    Ref<Mesh> Get(const std::string& key);
    bool Exists(const std::string& key) const;
    void Add(const std::string& key, const Ref<Mesh>& mesh);

private:
    std::unordered_map<std::string, Ref<Mesh>> m_MeshCache;
};

// MaterialLibrary.h
class MaterialLibrary {
public:
    static Ref<MaterialLibrary> Get(); // 获取单例

    Ref<Material> Get(const std::string& key);
    bool Exists(const std::string& key) const;
    void Add(const std::string& key, const Ref<Material>& material);

private:
    std::unordered_map<std::string, Ref<Material>> m_MaterialCache;
};
```

#### 2\. 设计唯一标识 (Key)

这是最关键的一步。一个简单有效的 Key 生成策略是**字符串拼接**。

  * **Material Key**: `model_path + "::mat::" + material_name_or_index`

      * 示例: `"assets/models/cyborg.fbx::mat::MetalBody"`
      * 示例: `"assets/models/cyborg.fbx::mat::2"` (如果材质没有名字，使用它在 aiScene 中的索引)

  * **Mesh Key**: `model_path + "::mesh::" + mesh_name_or_index`

      * 示例: `"assets/models/cyborg.fbx::mesh::Head"`
      * 示例: `"assets/models/cyborg.fbx::mesh::0"` (如果网格没有名字，使用它在 aiScene 中的索引)

这种方式保证了只要是源自同一个文件、同一个位置的资源，它们就会拥有完全相同的 Key。

### 第三步：修改加载流程

现在，我们需要重构之前的 `Model::Load` 流程，使其与新的资源库协同工作。同时，我们采纳之前的建议，引入 `Node` 结构。

#### 修改后的数据结构

```cpp
// Node.h
class Node {
    std::string m_Name;
    glm::mat4 m_LocalTransform;
    std::vector<Ref<Node>> m_Children;
    std::vector<Ref<Mesh>> m_Meshes; // 注意这里是 Ref<Mesh>
public:
    // ...
};

// Model.h
class Model {
    Ref<Node> m_RootNode;
public:
    static Ref<Model> Create(const std::string& path); // 使用静态工厂函数
private:
    bool Load(const std::string& path); // 变为私有
};
```

注意：`Model` 类现在变得非常轻量，它本身不再存储一大堆 Mesh，只持有场景图的根节点。真正的资源数据被转移到了资源库中。

#### 修改后的加载伪代码

```cpp
// Model.cpp
bool Model::Load(const std::string& modelPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, ...);
    if (!scene || ...) return false;

    // 递归处理节点，构建我们自己的 Node 树
    m_RootNode = ProcessNode(scene->mRootNode, scene, modelPath);
    return true;
}

Ref<Node> Model::ProcessNode(aiNode* assimpNode, const aiScene* scene, const std::string& modelPath) {
    Ref<Node> node = CreateRef<Node>();
    node->m_Name = assimpNode->mName.C_Str();
    node->m_LocalTransform = ConvertAssimpMatrix(assimpNode->mTransformation);

    // 1. 处理该节点拥有的所有 Mesh
    for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
        unsigned int meshIndex = assimpNode->mMeshes[i];
        aiMesh* assimpMesh = scene->mMeshes[meshIndex];

        // --- Mesh 资源管理 ---
        std::string meshKey = modelPath + "::mesh::" + std::to_string(meshIndex);
        Ref<Mesh> mesh;

        if (MeshLibrary::Get()->Exists(meshKey)) {
            // 如果 Mesh 已在缓存中，直接获取
            mesh = MeshLibrary::Get()->Get(meshKey);
        } else {
            // 如果不在缓存中，创建新的 Mesh
            mesh = ProcessMesh(assimpMesh, scene, modelPath); // 这是一个新函数
            // 添加到缓存库，供下次使用
            MeshLibrary::Get()->Add(meshKey, mesh);
        }
        node->m_Meshes.push_back(mesh);
    }

    // 2. 递归处理所有子节点
    for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
        node->m_Children.push_back(ProcessNode(assimpNode->mChildren[i], scene, modelPath));
    }

    return node;
}

// 新增的 ProcessMesh 函数
Ref<Mesh> ProcessMesh(aiMesh* assimpMesh, const aiScene* scene, const std::string& modelPath) {
    // 提取顶点、索引数据...
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    // ... (和原方案一样，填充这两个 vector) ...

    // --- Material 资源管理 ---
    unsigned int materialIndex = assimpMesh->mMaterialIndex;
    std::string materialKey = modelPath + "::mat::" + std::to_string(materialIndex);
    Ref<Material> material;

    if (MaterialLibrary::Get()->Exists(materialKey)) {
        // 如果 Material 已在缓存中，直接获取
        material = MaterialLibrary::Get()->Get(materialKey);
    } else {
        // 如果不在缓存中，创建新的 Material
        aiMaterial* assimpMaterial = scene->mMaterials[materialIndex];
        material = LoadMaterial(assimpMaterial, modelPath); // 这是加载材质和纹理的函数
        // 添加到缓存库
        MaterialLibrary::Get()->Add(materialKey, material);
    }

    // 创建 Mesh 对象，传入顶点/索引数据和获取到的 Material
    return CreateRef<Mesh>(vertices, indices, material);
}

// 新增的 LoadMaterial 函数
Ref<Material> LoadMaterial(aiMaterial* assimpMaterial, const std::string& modelPath) {
    Ref<Material> material = CreateRef<Material>();

    // 假设 Material 需要一个漫反射贴图
    aiString texturePath;
    if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
        // 从模型文件所在目录解析出完整的纹理路径
        std::string fullTexturePath = GetDirectoryFromPath(modelPath) + "/" + texturePath.C_Str();
        
        // **通过 TextureLibrary 获取纹理**
        Ref<Texture2D> diffuseMap = TextureLibrary::Get()->Load(fullTexturePath);
        material->SetDiffuseMap(diffuseMap);
    }
    
    // ... 加载其他纹理（法线、高光等）和颜色参数 ...

    return material;
}
```

### 总结与优势

通过以上改造，我们成功地将 `Mesh` 和 `Material` 也纳入了资源管理系统。

**带来的好处是巨大的：**

1.  **显著节省内存和显存**：如果一个场景里有 100 棵相同的树，它们的 `Mesh` 数据和 `Material` 数据在内存和显存中都只会存在一份。
2.  **提升加载速度**：当加载第二个使用相同资源的模型时，由于其 `Mesh` 和 `Material` 已经在缓存中，加载过程会变得飞快，几乎只剩下构建 `Node` 树的开销。
3.  **架构更加健壮**：资源和实例（场景中的对象）被清晰地分离开。`Model` 和 `Node` 负责组织场景结构和变换，而 `MeshLibrary`, `MaterialLibrary`, `TextureLibrary` 负责管理实际的数据。这使得系统更容易维护和扩展（例如，实现运行时的资源热重载）。
4.  **数据一致性**：修改一个 `Material` 资源（比如改变它的颜色），所有使用这个 `Material` 的物体都会立即同步更新，因为它们引用的都是同一个对象。