/**
 * @file VulkanTexture.h
 */
#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace chai::gfx
{
    struct GpuTexture {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        uint32_t width = 0, height = 0;
        VkFormat format = VK_FORMAT_UNDEFINED;
    };

    struct RenderTarget2D {
        GpuTexture tex;
        VkImageView view{};
    };

    struct CubeRenderTarget {
        GpuTexture cube;
        VkImageView faceViews[6]{};
    };

    struct PrefilterTarget {
        GpuTexture cube;
        std::vector<VkImageView> faceMipViews;
        uint32_t baseSize = 0;
        uint32_t mipCount = 0;
    };
} // namespace chai::gfx