#pragma once
#include <Loaders/ImageLoader.h>
#include "Buffer.h"
#include <AssetCache.h>
#include "VulkanContext.h"
#include "VkUtils.h"
#include "ImageTransition.h"
#include <FileIO.h>

namespace chai::gfx
{
    struct GpuTexture {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        VkDescriptorSet set = VK_NULL_HANDLE;
        uint32_t width = 0, height = 0;
        VkFormat format = VK_FORMAT_UNDEFINED;
    };
} // namespace chai::gfx

namespace chai
{
    template <>
    struct AssetTraits<Texture> {
        using Asset = TextureAsset;
        using Resource = gfx::GpuTexture;
    };
} // namespace chai

namespace chai::gfx
{
    class TextureFactory final : public ResourceFactory<Texture>
    {
    public:
        TextureFactory(VulkanContext* ctx) : ctx_(ctx)
        {
        }

        bool loadAsset(AssetId id, TextureAsset& out) override { return out.isValid(); }

        LoadState createResource(const TextureAsset& asset, GpuTexture& out) override
        {
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("Texture asset is invalid. Could not create Texture Resource.");
                return LoadState::Failed;
            }

            VkDevice device = ctx_->device();
            VmaAllocator allocator = ctx_->allocator();

            const VkFormat format = convertTextureFormat(asset.format); 
            const VkExtent3D extent{asset.width, asset.height, 1};

            const VkDeviceSize imageSize =
                asset.width * asset.height * asset.channels * asset.bytesPerChannel;

            out.width = asset.width;
            out.height = asset.height;
            out.format = format;

            //Image on the device
            VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = format;
            imageInfo.extent = extent;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VmaAllocationCreateInfo imgAlloc{};
            imgAlloc.usage = VMA_MEMORY_USAGE_AUTO;
            if (vmaCreateImage(allocator, &imageInfo, &imgAlloc, &out.image, &out.alloc, nullptr) !=
                VK_SUCCESS) {
                CHAI_LOG_ERROR("vmaCreateImage failed");
                return LoadState::Failed;
            }

            //Staging buffer for the host
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

            //another synchronous point to be removed
            immediateSubmit(*ctx_, [&](VkCommandBuffer cmd) {
                imageBarrier(cmd,
                             out.image,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                             0,
                             VK_PIPELINE_STAGE_2_COPY_BIT,
                             VK_ACCESS_2_TRANSFER_WRITE_BIT);

                VkBufferImageCopy region{};
                region.bufferOffset = 0;
                region.bufferRowLength = 0; // 0 = tightly packed
                region.bufferImageHeight = 0;
                region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
                region.imageExtent = extent;
                vkCmdCopyBufferToImage(
                    cmd, staging, out.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                imageBarrier(cmd,
                             out.image,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             VK_PIPELINE_STAGE_2_COPY_BIT,
                             VK_ACCESS_2_TRANSFER_WRITE_BIT,
                             VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                             VK_ACCESS_2_SHADER_READ_BIT);
            });

            vmaDestroyBuffer(allocator, staging, stagingAlloc);

            VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            viewInfo.image = out.image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
            if (vkCreateImageView(device, &viewInfo, nullptr, &out.view) != VK_SUCCESS) {
                CHAI_LOG_ERROR("vkCreateImageView failed");
                return LoadState::Failed;
            }

            //one sampler per texture, but this is bad
            VkSamplerCreateInfo sampInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
            sampInfo.magFilter = VK_FILTER_LINEAR;
            sampInfo.minFilter = VK_FILTER_LINEAR;
            sampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            vkCreateSampler(device, &sampInfo, nullptr, &out.sampler);

            //
            VkDescriptorSetLayout matLayout = ctx_->materialSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_->descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &matLayout;
            if (vkAllocateDescriptorSets(device, &dsai, &out.set) != VK_SUCCESS) {
                CHAI_LOG_ERROR("descriptor set alloc failed (pool exhausted?)");
                return LoadState::Failed;
            }

            VkDescriptorImageInfo imgDesc{};
            imgDesc.sampler = out.sampler;
            imgDesc.imageView = out.view;
            imgDesc.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            write.dstSet = out.set;
            write.dstBinding = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.pImageInfo = &imgDesc;
            vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

            return LoadState::Ready;
        }

        void destroyResource(GpuTexture& res) noexcept override
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

        ~TextureRegistry() override
        { 
            locator_ = nullptr;
        }

        std::shared_ptr<AssetCache<Texture>> cache() { return cache_; }

        Handle<Texture> ingest(AssetId id, TextureAsset asset) override
        { 
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("ingest: invalid TextureAsset for '{}'", id.value);
                return {};
            }
            return cache_->ingest(id, std::move(asset));
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

            auto image = loader->decode(bytes);
            if (!image)
                return {};

            return ingest(id, std::move(*image));
        }

        void release(Handle<Texture> h) override
        { 
            cache_->release(h);
        }

    private:
        std::shared_ptr<AssetCache<Texture>> cache_;
        ServiceLocator* locator_;
    };
}