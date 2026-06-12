#pragma once
#include <SceneExport.h>
#include <Assets/MeshAsset.h>
#include <Components/ComponentBase.h>
#include <Rendering/FrameRenderData.h>
#include <Loaders/ITextureLoader.h>

namespace chai::scene
{
    class SCENE_EXPORT MeshComponent : public Component
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void extract(gfx::FrameRenderData& frame) const override;

        void setMesh(Handle<gfx::Mesh> m) { mesh_ = m; }
        void setTexture(Handle<gfx::Texture> t) { texture_ = t; }
        void setMaterial(uint32_t id) { materialId_ = id; }

    private:
        Handle<gfx::Mesh> mesh_;
        Handle<gfx::Texture> texture_;
        math::Vec4 color_{1, 0, 1, 1};
        uint32_t materialId_ = 0;
    };
}