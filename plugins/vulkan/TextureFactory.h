#pragma once
#include "Buffer.h"
#include "ImageTransition.h"
#include "VkUtils.h"
#include "VulkanContext.h"

#include <AssetCache.h>
#include <Assets/ITextureRegistry.h>
#include <Assets/TextureAsset.h>
#include <Core/FileIO.h>
#include "VulkanTexture.h"

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
    class TextureFactory final : public ResourceFactory<Texture>
    {
    public:
        TextureFactory(VulkanContext* ctx) : ctx_(ctx) {}

        LoadState createResource(const gfx::TextureAsset& asset, gfx::GpuTexture& out) override
        {
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("Texture asset is invalid. Could not create Texture Resource.");
                return LoadState::Failed;
            }

            VkDevice device = ctx_->device();
            VmaAllocator allocator = ctx_->allocator();

            const VkFormat format = convertTextureFormat(asset.format);
            const VkExtent3D extent{asset.width, asset.height, 1};

            const VkDeviceSize layerSize =
                asset.width * asset.height * asset.channels * asset.bytesPerChannel;
            const VkDeviceSize imageSize = layerSize * asset.layerCount;

            out.width = asset.width;
            out.height = asset.height;
            out.format = format;

            const uint32_t mipCount =
                asset.isCube ? 1u
                             : static_cast<uint32_t>(
                                   std::floor(std::log2(std::max(asset.width, asset.height)))) +
                                   1;

            // Image on the device
            VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = format;
            imageInfo.extent = extent;
            imageInfo.mipLevels = mipCount;
            imageInfo.arrayLayers = asset.layerCount;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (asset.isCube) {
                imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            }

            VmaAllocationCreateInfo imgAlloc{};
            imgAlloc.usage = VMA_MEMORY_USAGE_AUTO;
            if (vmaCreateImage(allocator, &imageInfo, &imgAlloc, &out.image, &out.alloc, nullptr) !=
                VK_SUCCESS) {
                CHAI_LOG_ERROR("vmaCreateImage failed");
                return LoadState::Failed;
            }

            // Staging buffer for the host
            VkBufferCreateInfo stageInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            stageInfo.size = imageSize;
            stageInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo stageAlloc{};
            stageAlloc.usage = VMA_MEMORY_USAGE_AUTO;
            stageAlloc.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                               VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VkBuffer staging;
            VmaAllocation stagingAlloc;
            VmaAllocationInfo stagingInfo;
            if (vmaCreateBuffer(
                    allocator, &stageInfo, &stageAlloc, &staging, &stagingAlloc, &stagingInfo) !=
                VK_SUCCESS) {
                vmaDestroyImage(allocator, out.image, out.alloc);
                out.image = VK_NULL_HANDLE;
                out.alloc = VK_NULL_HANDLE;
                CHAI_LOG_ERROR("staging buffer alloc failed");
                return LoadState::Failed;
            }
            std::memcpy(stagingInfo.pMappedData, asset.pixels.data(), imageSize);

            // another synchronous point to be removed
            const uint32_t layers = asset.layerCount;

            immediateSubmit(*ctx_, [&](VkCommandBuffer cmd) {
                imageBarrier(cmd,
                             out.image,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                             0,
                             VK_PIPELINE_STAGE_2_COPY_BIT,
                             VK_ACCESS_2_TRANSFER_WRITE_BIT,
                             VK_IMAGE_ASPECT_COLOR_BIT);

                // one copy region PER FACE
                std::vector<VkBufferImageCopy> regions(layers);
                for (uint32_t f = 0; f < layers; ++f) {
                    regions[f].bufferOffset = VkDeviceSize(f) * layerSize;
                    regions[f].bufferRowLength = 0;
                    regions[f].bufferImageHeight = 0;
                    regions[f].imageSubresource = {
                        VK_IMAGE_ASPECT_COLOR_BIT, 0 /*mip*/, f /*baseArrayLayer*/, 1};
                    regions[f].imageExtent = extent;
                }
                vkCmdCopyBufferToImage(cmd,
                                       staging,
                                       out.image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       layers,
                                       regions.data());

                if (asset.isCube) {
                    imageBarrier(cmd,
                                 out.image,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 VK_PIPELINE_STAGE_2_COPY_BIT,
                                 VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                 VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                                 VK_ACCESS_2_SHADER_READ_BIT,
                                 VK_IMAGE_ASPECT_COLOR_BIT /*, all layers */);
                } else {
                    generateMipmaps(
                        cmd, out.image, int32_t(asset.width), int32_t(asset.height), mipCount);
                }
            });

            vmaDestroyBuffer(allocator, staging, stagingAlloc);

            VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            viewInfo.image = out.image;
            viewInfo.viewType = asset.isCube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT, 0, mipCount, 0, asset.layerCount};
            if (vkCreateImageView(device, &viewInfo, nullptr, &out.view) != VK_SUCCESS) {
                CHAI_LOG_ERROR("vkCreateImageView failed");
                return LoadState::Failed;
            }

            VkSamplerAddressMode mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            if (asset.isCube) {
                mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            }

            // one sampler per texture, but this is bad
            VkSamplerCreateInfo sampInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
            sampInfo.magFilter = VK_FILTER_LINEAR;
            sampInfo.minFilter = VK_FILTER_LINEAR;
            sampInfo.addressModeU = mode;
            sampInfo.addressModeV = mode;
            sampInfo.addressModeW = mode;
            sampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            sampInfo.minLod = 0.0f;
            sampInfo.maxLod = static_cast<float>(mipCount);
            sampInfo.anisotropyEnable = VK_TRUE;
            sampInfo.maxAnisotropy = 8.0f;
            vkCreateSampler(device, &sampInfo, nullptr, &out.sampler);

            return LoadState::Ready;
        }

        void destroyResource(gfx::GpuTexture& res) noexcept override
        {
            VkDevice device = ctx_->device();
            if (res.sampler)
                vkDestroySampler(device, res.sampler, nullptr);
            if (res.view)
                vkDestroyImageView(device, res.view, nullptr);
            if (res.image)
                vmaDestroyImage(ctx_->allocator(), res.image, res.alloc);
        }

        bool discardAssetAfterUpload() const noexcept override { return true; }

    private:
        VulkanContext* ctx_ = nullptr;
    };

    // concrete impl
    class TextureRegistry : public ITextureRegistry
    {
    public:
        TextureRegistry(ResourceFactory<Texture>& factory,
                        DeferredDeleteQueue& graveyard,
                        ServiceLocator* locator)
            : locator_(locator)
        {
            cache_ = std::make_shared<AssetCache<Texture>>(factory, graveyard);
        }

        ~TextureRegistry() override { locator_ = nullptr; }

        std::shared_ptr<AssetCache<Texture>> cache() { return cache_; }

        Handle<Texture> ingest(AssetId id, TextureAsset asset) override
        {
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("ingest: invalid TextureAsset for '{}'", id.value);
                return {};
            }
            return cache_->ingest(id, std::move(asset));
        }

        Handle<Texture> loadCubemap(AssetId id,
                                    const std::array<std::filesystem::path, 6>& files) override
        {
            auto loader = locator_->tryResolve<ITextureLoader>();
            if (!loader) {
                CHAI_LOG_ERROR("No image loader for cubemap");
                return {};
            }

            TextureAsset cube{};
            cube.isCube = true;
            cube.layerCount = 6;

            for (size_t i = 0; i < 6; ++i) {
                auto bytes = readFileBytes(files[i]);
                if (bytes.empty()) {
                    CHAI_LOG_ERROR("Could not read cubemap face {}", files[i].string());
                    return {};
                }
                auto image = loader->decode(bytes, TextureFormat::RGBA8_SRGB);
                if (!image)
                    return {};

                if (i == 0) {
                    cube.width = image->width;
                    cube.height = image->height;
                    cube.channels = image->channels;
                    cube.bytesPerChannel = image->bytesPerChannel;
                    cube.format = image->format;
                    cube.pixels.reserve(image->pixels.size() * 6);
                } else if (image->width != cube.width || image->height != cube.height) {
                    CHAI_LOG_ERROR("cubemap face {} size mismatch ({}x{} vs {}x{})",
                                   i,
                                   image->width,
                                   image->height,
                                   cube.width,
                                   cube.height);
                    return {};
                }
                // append this faces bytes
                cube.pixels.insert(cube.pixels.end(), image->pixels.begin(), image->pixels.end());
            }
            return ingest(id, std::move(cube));
        }

        Handle<Texture> load(AssetId id, const std::filesystem::path& path) override
        {
            auto bytes = readFileBytes(path);
            if (bytes.empty()) {
                return {};
            }

            auto ext = path.extension().string();
            if (!ext.empty() && ext.front() == '.')
                ext.erase(0, 1);

            auto loader = locator_->tryResolve<ITextureLoader>();
            if (!loader) {
                CHAI_LOG_ERROR("No image loader for '.{}' ('{}')", ext, path.string());
                return {};
            }

            auto image = loader->decode(bytes, TextureFormat::RGBA8_SRGB);
            if (!image)
                return {};

            return ingest(id, std::move(*image));
        }

        void release(Handle<Texture> h) override { cache_->release(h); }

    private:
        std::shared_ptr<AssetCache<Texture>> cache_;
        ServiceLocator* locator_;
    };
} // namespace chai::gfx