#pragma once
#include <SceneExport.h>
#include <Assets/MeshAsset.h>
#include <Rendering/FrameRenderData.h>
#include <Loaders/ITextureLoader.h>
#include <Components/Component.h>
#include <Scene/IUpdatable.h>

namespace chai::scene
{
    class SCENE_EXPORT MeshComponent : public Component, public IUpdatable
    {
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        Handle<gfx::Mesh> getMesh() const { return mesh_; }
        void setMesh(Handle<gfx::Mesh> m) { mesh_ = m; }

        Handle<gfx::Material> getMaterial() const { return material_; }
        void setMaterial(Handle<gfx::Material> mat) { material_ = mat; }

    private:
        Handle<gfx::Mesh> mesh_;
        Handle<gfx::Material> material_;
    };

    CHAI_REFLECT(MeshComponent, "MeshComponent")
    {
        CHAI_ICON(ICON_FA_CUBES);
    }
}
