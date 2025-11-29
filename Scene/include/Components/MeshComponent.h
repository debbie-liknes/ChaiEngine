#pragma once
#include <SceneExport.h>
#include <Components/RenderableComponent.h>
#include <ChaiEngine/IMesh.h>
#include <Asset/AssetHandle.h>
#include <ChaiEngine/PipelineState.h>

namespace chai::cup
{
    class SCENE_EXPORT MeshComponent : public RenderableComponent
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void setMesh(AssetHandle meshAsset);
        ResourceHandle getMesh() { return m_meshResource; }
        ResourceHandle getMesh() const { return m_meshResource; }

        // Override material for all submeshes
        void setMaterial(AssetHandle material);
        void setMaterial(ResourceHandle material);
        void setMaterialInstance(ResourceHandle material);

        void setMaterial(size_t submeshIndex, AssetHandle material);
        void setMaterial(size_t submeshIndex, ResourceHandle material);

        ResourceHandle getMaterial(int i)
        {
            if (m_useSingleMaterial)
            {
                if (m_materialInstance.isValid())
                    return m_materialInstance;
                else
                    return m_singleMaterial;
            }

            return m_materialOverrides[i];
        }

        PipelineState& getPipelineState() { return m_pipelineState; }

    private:
        static ResourceHandle createMeshResourceFromAsset(const MeshAsset* asset);
        static ResourceHandle createMeshResourceFromAsset(AssetHandle asset);

        static ResourceHandle createMaterialResourceFromAsset(AssetHandle asset);

        ResourceHandle m_meshResource;

        PipelineState m_pipelineState;

        std::unordered_map<size_t, ResourceHandle> m_materialOverrides;
        ResourceHandle m_singleMaterial;
        bool m_useSingleMaterial = false;
        ResourceHandle m_materialInstance;
    };
}