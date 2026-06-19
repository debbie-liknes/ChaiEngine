#include "MaterialRegistry.h"

namespace chai::gfx
{
    MaterialRegistry::MaterialRegistry(VulkanContext& ctx,
                                       std::shared_ptr<AssetCache<Texture>> texCache,
                                       DeferredDeleteQueue& graveyard)
        : texCache_(texCache)
    {
        defaultWhite_ = texCache_->ingest(makeAssetId("builtin:white"), createWhiteTexture());
        defaultNormal_ =
            texCache_->ingest(makeAssetId("builtin:normal"), createDefaultNormalTexture());

        factory_ =
            std::make_unique<MaterialFactory>(&ctx, texCache_, defaultWhite_, defaultNormal_);
        cache_ = std::make_shared<AssetCache<Material>>(*factory_, graveyard);
    }

    Handle<Material> MaterialRegistry::ingest(AssetId id, MaterialAsset asset)
    {
        return cache_->ingest(id, asset);
    }

    Handle<Material> MaterialRegistry::load(AssetId id)
    {
        return cache_->acquire(id);
    }

    void MaterialRegistry::release(Handle<Material> h)
    {
        cache_->release(h);
    }

    Handle<Material> MaterialRegistry::defaultMaterial()
    {
        if (!defaultMatBuilt_) {
            defaultMat_ = cache_->ingest(makeAssetId("builtin:material:default"), MaterialAsset{});
            defaultMatBuilt_ = true;
        } else {
            cache_->addRef(defaultMat_);
        }
        return defaultMat_;
    }

    std::shared_ptr<AssetCache<Material>> MaterialRegistry::cache()
    {
        return cache_;
    }
} // namespace chai::gfx