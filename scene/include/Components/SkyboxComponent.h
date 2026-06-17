#pragma once
#include <SceneExport.h>
#include <Rendering/FrameRenderData.h>
#include <Loaders/ITextureLoader.h>
#include <Assets/TextureAsset.h>
#include <Components/Component.h>
#include <Updatable.h>

namespace chai::scene
{
    class SkyboxComponent : public Component, public IUpdatable
    {
    public:
        explicit SkyboxComponent(GameObject* owner = nullptr);
        ~SkyboxComponent() override;

        void extract(gfx::FrameRenderData& frame) const override;
        void setTexture(Handle<gfx::Texture> h) { skyMap_ = h; }

    private:
        Handle<gfx::Texture> skyMap_;
    };
}