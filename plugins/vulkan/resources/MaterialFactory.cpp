#include "MaterialFactory.h"

namespace chai::gfx
{
    MaterialFactory::MaterialFactory(VulkanContext* ctx,
                                     std::shared_ptr<AssetCache<Texture>> texCache,
                                     Handle<Texture> white,
                                     Handle<Texture> normal)
        : ctx_(ctx), textureCache_(texCache), defaultWhite_(white), defaultNormal_(normal)
    {
    }

    LoadState MaterialFactory::createResource(const gfx::MaterialAsset& asset,
                                              gfx::GpuMaterial& out)
    {
        VkDevice device = ctx_->device();
        VmaAllocator allocator = ctx_->allocator();

        // Extract the factors from cpu asset, allocate a UBO to store them
        MaterialFactors f{};
        f.baseColor = asset.baseColorFactor;
        f.emissive =
            math::Vec4{asset.emissiveFactor.x, asset.emissiveFactor.y, asset.emissiveFactor.z, 0.f};
        f.metallic = asset.metallic;
        f.roughness = asset.roughness;
        f.alphaCutoff = (asset.alphaMode == AlphaMode::Mask) ? asset.alphaCutoff : 0.f;

        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.size = sizeof(MaterialFactors);
        bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo aci{};
        aci.usage = VMA_MEMORY_USAGE_AUTO;
        aci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo{};
        if (vmaCreateBuffer(
                allocator, &bufInfo, &aci, &out.factorsBuffer, &out.factorsAlloc, &allocInfo) !=
            VK_SUCCESS) {
            CHAI_LOG_ERROR("material factors UBO alloc failed");
            return LoadState::Failed;
        }
        std::memcpy(allocInfo.pMappedData, &f, sizeof(f));

        // Alloc from descriptor set pool
        VkDescriptorSetLayout layout = ctx_->materialSetLayout();
        VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        dsai.descriptorPool = ctx_->descriptorPool();
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &layout;
        if (vkAllocateDescriptorSets(device, &dsai, &out.set) != VK_SUCCESS) {
            vmaDestroyBuffer(allocator, out.factorsBuffer, out.factorsAlloc);
            out.factorsBuffer = VK_NULL_HANDLE;
            out.factorsAlloc = VK_NULL_HANDLE;
            CHAI_LOG_ERROR("material descriptor set alloc failed (pool might be exhausted?)");
            return LoadState::Failed;
        }

        // Fill out the Gpu mat
        out.baseColor = asset.baseColor;
        out.metallicRoughness = asset.metallicRoughness;
        out.normal = asset.normal;
        out.occlusion = asset.occlusion;
        out.emissive = asset.emissive;
        out.alphaMode = asset.alphaMode;
        out.doubleSided = asset.doubleSided;

        const GpuTexture& base = resolveOrDefault(asset.baseColor, defaultWhite_);
        const GpuTexture& mr = resolveOrDefault(asset.metallicRoughness, defaultWhite_);
        const GpuTexture& normal = resolveOrDefault(asset.normal, defaultNormal_);
        const GpuTexture& occlusion = resolveOrDefault(asset.occlusion, defaultWhite_);
        const GpuTexture& emissive = resolveOrDefault(asset.emissive, defaultWhite_);

        // 5 textures
        VkDescriptorBufferInfo dbi{};
        dbi.buffer = out.factorsBuffer;
        dbi.offset = 0;
        dbi.range = sizeof(MaterialFactors);

        const GpuTexture* texPtr[5] = {&base, &mr, &normal, &occlusion, &emissive};
        VkDescriptorImageInfo imgInfo[5]{};
        for (int i = 0; i < 5; i++) {
            imgInfo[i].imageView = texPtr[i]->view;
            imgInfo[i].sampler = texPtr[i]->sampler;
            imgInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        VkWriteDescriptorSet writes[6]{};
        writes[0] = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writes[0].dstSet = out.set;
        writes[0].dstBinding = 0;
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &dbi;
        for (int i = 0; i < 5; ++i) {
            writes[i + 1] = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            writes[i + 1].dstSet = out.set;
            writes[i + 1].dstBinding = uint32_t(i + 1);
            writes[i + 1].descriptorCount = 1;
            writes[i + 1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[i + 1].pImageInfo = &imgInfo[i];
        }
        vkUpdateDescriptorSets(device, 6, writes, 0, nullptr);

        return LoadState::Ready;
    }

    void MaterialFactory::destroyResource(gfx::GpuMaterial& m) noexcept
    {
        if (m.factorsBuffer)
            vmaDestroyBuffer(ctx_->allocator(), m.factorsBuffer, m.factorsAlloc);
        m.factorsBuffer = VK_NULL_HANDLE;
        m.factorsAlloc = VK_NULL_HANDLE;

        // release the texture references this material owned (ownership transfer)
        if (textureCache_) {
            textureCache_->release(m.baseColor);
            textureCache_->release(m.metallicRoughness);
            textureCache_->release(m.normal);
            textureCache_->release(m.occlusion);
            textureCache_->release(m.emissive);
        }
    }

    bool MaterialFactory::discardAssetAfterUpload() const noexcept
    {
        return true;
    }

    const GpuTexture& MaterialFactory::resolveOrDefault(Handle<Texture> tex,
                                                        Handle<Texture> fallback)
    {
        if (const GpuTexture* t = textureCache_->resource(tex))
            return *t;
        return *textureCache_->resource(fallback);
    }
} // namespace chai::gfx