#pragma once
#include "../renderer/VulkanContext.h"
#include "VulkanTexture.h"

#include <vector>

namespace chai::gfx
{
    // struct RenderTarget2D {
    //     GpuTexture tex;
    //     VkImageView view{};
    // };

    // struct CubeRenderTarget {
    //     GpuTexture cube;
    //     VkImageView faceViews[6]{};
    // };

    // struct PrefilterTarget {
    //     GpuTexture cube;
    //     std::vector<VkImageView> faceMipViews;
    //     uint32_t baseSize = 0;
    //     uint32_t mipCount = 0;
    // };

    struct RenderTarget {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE; // sampling view
        VkSampler sampler = VK_NULL_HANDLE;
        std::vector<VkImageView> renderViews;

        uint32_t width = 0, height = 0;
        uint32_t mipCount = 1, layerCount = 1;
        VkFormat format = VK_FORMAT_UNDEFINED;
        bool isCube = false;

        // attachment view for a given (mip, face); matches your prefilter indexing
        VkImageView renderView(uint32_t mip = 0, uint32_t face = 0) const
        {
            return renderViews[mip * layerCount + face];
        }

        void destroy(VulkanContext& ctx)
        {
            VkDevice d = ctx.device();
            if (sampler)
                vkDestroySampler(d, sampler, nullptr);
            if (view)
                vkDestroyImageView(d, view, nullptr);
            for (auto v : renderViews)
                if (v)
                    vkDestroyImageView(d, v, nullptr);
            if (image)
                vmaDestroyImage(ctx.allocator(), image, alloc);
            *this = {};
        }
    };

    RenderTarget createColor2D(VulkanContext& ctx, uint32_t w, uint32_t h, VkFormat fmt);
    RenderTarget createDepth2D(VulkanContext& ctx, uint32_t w, uint32_t h, VkFormat fmt, bool compare);
    RenderTarget createCube(VulkanContext& ctx, uint32_t size, VkFormat fmt, uint32_t mips);
} // namespace chai::gfx