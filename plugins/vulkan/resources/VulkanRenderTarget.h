/**
 * @file VulkanRenderTarget.h
 */
#pragma once
#include "../renderer/VulkanContext.h"
#include "VulkanTexture.h"

#include <vector>

namespace chai::gfx
{
    /**
     * @brief A view to render into. Can be presented to the screen, or sampled by later passes
     */
    struct RenderTarget {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE; // sampling view
        VkSampler sampler = VK_NULL_HANDLE;
        std::vector<VkImageView> renderViews;

        VkExtent2D extent{0, 0};
        uint32_t mipCount = 1, layerCount = 1;
        VkFormat format = VK_FORMAT_UNDEFINED;
        bool isCube = false;

        // depth
        VkImage depthImage = VK_NULL_HANDLE;
        VkImageView depthView = VK_NULL_HANDLE;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;

        VkClearValue clearColor{};

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

    ///////////////// Helper functions to create Render Targets ///////////////////

    RenderTarget createColor2D(VulkanContext& ctx,
                               uint32_t w,
                               uint32_t h,
                               VkFormat fmt,
                               uint32_t mips = 1,
                               VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    RenderTarget createDepth2D(VulkanContext& ctx,
                               uint32_t w,
                               uint32_t h,
                               VkFormat fmt,
                               bool compare,
                               VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    RenderTarget createCube(VulkanContext& ctx, uint32_t size, VkFormat fmt, uint32_t mips);
} // namespace chai::gfx