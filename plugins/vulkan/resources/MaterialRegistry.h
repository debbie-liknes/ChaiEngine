#pragma once
#include "../renderer/VulkanContext.h"
#include "MaterialFactory.h"

#include <Assets/DefaultTextures.h>
#include <Assets/IMaterialRegistry.h>

namespace chai::gfx
{
    class MaterialRegistry : public IMaterialRegistry
    {
    public:
        MaterialRegistry(VulkanContext& ctx,
                         std::shared_ptr<AssetCache<Texture>> texCache,
                         DeferredDeleteQueue& graveyard);
        ~MaterialRegistry() = default;

        Handle<Material> ingest(AssetId id, MaterialAsset asset) override;

        Handle<Material> load(AssetId id) override;

        void release(Handle<Material> h) override;

        Handle<Material> defaultMaterial() override;

        std::shared_ptr<AssetCache<Material>> cache();

    private:
        std::shared_ptr<AssetCache<Texture>> texCache_;
        std::unique_ptr<MaterialFactory> factory_;
        std::shared_ptr<AssetCache<Material>> cache_;
        Handle<Texture> defaultWhite_{};
        Handle<Texture> defaultNormal_{};
        Handle<Material> defaultMat_{};
        bool defaultMatBuilt_ = false;
    };
} // namespace chai::gfx