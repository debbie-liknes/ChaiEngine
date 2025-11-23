#pragma once
#include <SceneExport.h>
#include <Components/RenderableComponent.h>
#include <ChaiEngine/IMesh.h>
#include <Asset/AssetHandle.h>

namespace chai::cup
{
    class SCENE_EXPORT MeshComponent : public RenderableComponent
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void setMesh(AssetHandle meshAsset);
        void setMaterial(ResourceHandle material);
        ResourceHandle getMeshResource();

        ResourceHandle getMesh() const { return m_meshResource; }

        ResourceHandle getMaterialInstance() const
        {
            return m_materialInstance;
        }

        //private:
        // Somewhere in your material system
        static ResourceHandle createMeshResourceFromAsset(const MeshAsset* asset);
        //static ResourceHandle createMaterialResourceFromAsset(const MaterialAsset* asset);


    private:
        ResourceHandle m_meshResource;
        ResourceHandle m_materialResource;
        ResourceHandle m_materialInstance;
    };
}