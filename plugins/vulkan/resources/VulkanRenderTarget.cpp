#include "VulkanRenderTarget.h"
#include <Log.h>

namespace chai::gfx
{
    RenderTarget makeRenderTarget(VulkanContext& ctx,
                                         uint32_t w,
                                         uint32_t h,
                                         VkFormat format,
                                         uint32_t layers,
                                         uint32_t mips,
                                         VkImageUsageFlags usage,
                                         VkImageAspectFlags aspect,
                                         bool cube,
                                         VkSamplerAddressMode addressMode,
                                         bool compareSampler)
    {
        VkDevice device = ctx.device();
        RenderTarget t;
        t.width = w;
        t.height = h;
        t.format = format;
        t.mipCount = mips;
        t.layerCount = layers;
        t.isCube = cube;

        VkImageCreateInfo ii{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        ii.imageType = VK_IMAGE_TYPE_2D;
        ii.format = format;
        ii.extent = {w, h, 1};
        ii.mipLevels = mips;
        ii.arrayLayers = layers;
        ii.samples = VK_SAMPLE_COUNT_1_BIT;
        ii.tiling = VK_IMAGE_TILING_OPTIMAL;
        ii.usage = usage;
        ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (cube)
            ii.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        VmaAllocationCreateInfo ai{};
        ai.usage = VMA_MEMORY_USAGE_AUTO;
        if (vmaCreateImage(ctx.allocator(), &ii, &ai, &t.image, &t.alloc, nullptr) != VK_SUCCESS) {
            CHAI_LOG_ERROR("makeRenderTarget: vmaCreateImage failed");
            return {};
        }

        // sampling view: full range
        VkImageViewCreateInfo sv{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        sv.image = t.image;
        sv.viewType = cube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
        sv.format = format;
        sv.subresourceRange = {aspect, 0, mips, 0, layers};
        if (vkCreateImageView(device, &sv, nullptr, &t.view) != VK_SUCCESS) {
            CHAI_LOG_ERROR("makeRenderTarget: sampling view failed");
            return {};
        }

        // attachment views: one per (mip, layer)
        t.renderViews.resize(mips * layers);
        for (uint32_t mip = 0; mip < mips; ++mip)
            for (uint32_t layer = 0; layer < layers; ++layer) {
                VkImageViewCreateInfo av{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
                av.image = t.image;
                av.viewType = VK_IMAGE_VIEW_TYPE_2D; // attachments are always 2D, single layer
                av.format = format;
                av.subresourceRange = {aspect, mip, 1, layer, 1};
                if (vkCreateImageView(device, &av, nullptr, &t.renderViews[mip * layers + layer]) !=
                    VK_SUCCESS) {
                    CHAI_LOG_ERROR("makeRenderTarget: attachment view ({},{}) failed", mip, layer);
                    return {};
                }
            }

        VkSamplerCreateInfo s{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        s.magFilter = s.minFilter = VK_FILTER_LINEAR;
        s.addressModeU = s.addressModeV = s.addressModeW = addressMode;
        s.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        s.minLod = 0.0f;
        s.maxLod = float(mips - 1);
        if (compareSampler) { // sampler2DShadow / hardware PCF
            s.compareEnable = VK_TRUE;
            s.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        }
        if (vkCreateSampler(device, &s, nullptr, &t.sampler) != VK_SUCCESS) {
            CHAI_LOG_ERROR("makeRenderTarget: sampler failed");
            return {};
        }
        return t;
    }

    RenderTarget createColor2D(VulkanContext& ctx, uint32_t w, uint32_t h, VkFormat fmt)
    {
        return makeRenderTarget(ctx,
                                w,
                                h,
                                fmt,
                                1,
                                1,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_IMAGE_ASPECT_COLOR_BIT,
                                false,
                                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                false);
    }

    RenderTarget
    createDepth2D(VulkanContext& ctx, uint32_t w, uint32_t h, VkFormat fmt, bool compare)
    {
        return makeRenderTarget(ctx,
                                w,
                                h,
                                fmt,
                                1,
                                1,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                false,
                                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                                compare);
    }

    RenderTarget createCube(VulkanContext& ctx, uint32_t size, VkFormat fmt, uint32_t mips)
    {
        return makeRenderTarget(ctx,
                                size,
                                size,
                                fmt,
                                6,
                                mips,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_IMAGE_ASPECT_COLOR_BIT,
                                true,
                                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                false);
    }
}