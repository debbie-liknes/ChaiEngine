#pragma once
#include <SceneExport.h>
#include <Rendering/FrameRenderData.h>
#include <Loaders/ITextureLoader.h>
#include <Assets/TextureAsset.h>
#include <Components/Component.h>
#include <Scene/IUpdatable.h>

namespace chai::scene
{
    class SkyboxComponent : public Component, public IUpdatable
    {
    public:
        CHAI_OBJECT(SkyboxComponent)

        explicit SkyboxComponent(GameObject* owner = nullptr);
        ~SkyboxComponent() override;

        Handle<gfx::Texture> getTexture() const { return skyMap_; }
        void setTexture(Handle<gfx::Texture> h) { skyMap_ = h; }

    private:
        Handle<gfx::Texture> skyMap_;
    };

    CHAI_REFLECT(SkyboxComponent, "SkyboxComponent") 
    {
        CHAI_ICON(ICON_FA_CLOUD);
    }
}