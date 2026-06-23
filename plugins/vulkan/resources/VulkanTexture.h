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

        bool operator==(const GpuTexture&) const = default;
    };
} // namespace chai::gfx