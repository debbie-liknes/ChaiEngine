#pragma once
#include <SceneExport.h>
#include <Components/RenderableComponent.h>
#include <ChaiEngine/IMesh.h>
#include <Asset/AssetHandle.h>
#include <ChaiEngine/PipelineState.h>

namespace chai::scene
{
    class SCENE_EXPORT MeshComponent : public RenderableComponent
    {
    public:
    //    MeshComponent(GameObject* owner = nullptr);
    //    ~MeshComponent() override;

    //    void setMesh(Handle<MeshAsset> meshAsset);
    //    Handle<MeshResource> getMesh() { return m_meshResource; }
    //    Handle<MeshResource> getMesh() const { return m_meshResource; }

    //    // Override material for all submeshes
    //    void setMaterial(Handle<MaterialAsset> material);
    //    void setMaterial(Handle<MaterialResource> material);
    //    void setMaterialInstance(Handle<MaterialResource> material);

    //    void setMaterial(size_t submeshIndex, Handle<MaterialAsset> material);
    //    void setMaterial(size_t submeshIndex, Handle<MaterialResource> material);

    //    Handle<MaterialResource> getMaterial(int i)
    //    {
    //        if (m_useSingleMaterial)
    //        {
    //            if (!m_materialInstance.isNull())
    //                return m_materialInstance;
    //            else
    //                return m_singleMaterial;
    //        }

    //        return m_materialOverrides[i];
    //    }

    //    PipelineState& getPipelineState() { return m_pipelineState; }

    //private:
    //    static Handle<MeshResource> createMeshResourceFromAsset(const MeshAsset* asset);
    //    static Handle<MeshResource> createMeshResourceFromAsset(Handle<MeshAsset> asset);

    //    static Handle<MaterialResource> createMaterialResourceFromAsset(Handle<MaterialAsset> asset);

    //    Handle<MeshResource> m_meshResource;

    //    PipelineState m_pipelineState;

    //    std::unordered_map<size_t, Handle<MaterialResource>> m_materialOverrides;
    //    Handle<MaterialResource> m_singleMaterial;
    //    bool m_useSingleMaterial = false;
    //    Handle<MaterialResource> m_materialInstance;
    };
}