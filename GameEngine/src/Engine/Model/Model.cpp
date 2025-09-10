#include "pch.h"

#include "Model.h"

#include "ModelLoader.h"

namespace Engine {

Ref<Model> Model::Create(const std::string& path) {

    Ref<Model> model = CreateRef<Model>();

    if (ModelLoader::Load(path, model)) {

        return model;

    }

    return nullptr;

}

void Model::Draw() const {
    if (m_RootNode) {
        // 应用全局变换到根节点
        m_RootNode->Draw(m_GlobalTransform);
    }
}

void Model::SetObjectID(int objectID) const {
    if (m_RootNode) {
        m_RootNode->SetObjectID(objectID);
    }
}

}