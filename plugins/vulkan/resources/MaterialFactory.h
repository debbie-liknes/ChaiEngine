/**
 * @file MaterialFactory.h
 */
#pragma once
#include "../renderer/VulkanContext.h"
#include "Material.h"
#include "TextureFactory.h"

#include <AssetCache.h>
#include <Assets/DefaultTextures.h>
#include <Assets/MaterialAsset.h>
#include <Log.h>
#include <ResourceFactory.h>

namespace chai
{
    template <>
    struct AssetTraits<gfx::Material> {
        using Asset = gfx::MaterialAsset;
        using Resource = gfx::GpuMaterial;
    };
} // namespace chai

namespace chai::gfx
{
    /**
     * @brief Knows how to convert an asset (CPU) to a resource (GPU).
     * Manages the lifetime of the GPU material
     */
    class MaterialFactory : public ResourceFactory<Material>
    {
    public:
        MaterialFactory(VulkanContext* ctx,
                        std::shared_ptr<AssetCache<Texture>> texCache,
                        Handle<Texture> white,
                        Handle<Texture> normal);

        LoadState createResource(const gfx::MaterialAsset& asset, gfx::GpuMaterial& out) override;
        void destroyResource(gfx::GpuMaterial& m) noexcept override;
        bool discardAssetAfterUpload() const noexcept override;

        LoadState pollState(const GpuMaterial&) override;

    private:
        const GpuTexture& resolveOrDefault(Handle<Texture> tex, Handle<Texture> fallback);

        VulkanContext* ctx_;
        std::shared_ptr<AssetCache<Texture>> textureCache_;
        Handle<Texture> defaultWhite_;
        Handle<Texture> defaultNormal_;
    };
} // namespace chai::gfx