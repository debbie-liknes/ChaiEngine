#pragma once
#include <SceneExport.h>
#include <Assets/MeshAsset.h>
#include <Rendering/FrameRenderData.h>
#include <Loaders/ITextureLoader.h>
#include <Components/Component.h>
#include <Updatable.h>

namespace chai::scene
{
    class SCENE_EXPORT MeshComponent : public Component, public IUpdatable
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void extract(gfx::FrameRenderData& frame) const override;

        void setMesh(Handle<gfx::Mesh> m) { mesh_ = m; }
        void setTexture(Handle<gfx::Texture> t) { texture_ = t; }
        void setMaterial(Handle<gfx::Material> mat) { material_ = mat; }

    private:
        Handle<gfx::Mesh> mesh_;
        Handle<gfx::Texture> texture_;
        math::Vec4 color_{1, 0, 1, 1};
        Handle<gfx::Material> material_;
    };
}