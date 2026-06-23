/**
 * @file TextureFactory.h
 */
#pragma once
#include "../commands/ImageTransition.h"
#include "../renderer/VulkanContext.h"
#include "../utils/VkUtils.h"
#include "Buffer.h"
#include "VulkanTexture.h"

#include <AssetCache.h>
#include <Assets/TextureAsset.h>
#include <Core/FileIO.h>
#include <Plugin/ServiceLocator.h>

namespace chai
{
    template <>
    struct AssetTraits<gfx::Texture> {
        using Asset = gfx::TextureAsset;
        using Resource = gfx::GpuTexture;
    };
} // namespace chai

namespace chai::gfx
{
    /**
     * @brief Creates a GPU resource frpm a CPU texture asset
     */
    class TextureFactory final : public ResourceFactory<Texture>
    {
    public:
        TextureFactory(VulkanContext* ctx);

        LoadState createResource(const gfx::TextureAsset& asset, gfx::GpuTexture& out) override;
        void destroyResource(gfx::GpuTexture& res) noexcept override;
        bool discardAssetAfterUpload() const noexcept override;

        LoadState pollState(const gfx::GpuTexture&) override;

    private:
        VulkanContext* ctx_ = nullptr;

        struct PendingUpload {
            GpuTexture tex;
            uint64_t value;
            VmaAllocation alloc;
            VkBuffer staging;
        };
        std::vector<PendingUpload> pending_;
    };
} // namespace chai::gfx