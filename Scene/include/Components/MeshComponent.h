#pragma once
#include <SceneExport.h>
#include <MeshAsset.h>
#include <Components/ComponentBase.h>
#include <FrameRenderData.h>
#include <MeshAsset.h>

namespace chai::scene
{
    class SCENE_EXPORT MeshComponent : public Component
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void extract(gfx::FrameRenderData& frame) const override;

        void setMesh(Handle<gfx::Mesh> m) { mesh_ = m; }

    private:
        Handle<gfx::Mesh> mesh_;
        math::Vec4 color_{1, 0, 1, 1};
    };
}