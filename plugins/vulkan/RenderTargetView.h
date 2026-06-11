/**
 * @file RenderTargetView.h
 * @brief Description of where to draw this frame.
 */
#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    /**
     * @brief Keeps the scene and the swaapchain separate
     */
    struct RenderTargetView {
        VkExtent2D extent{};

        VkImage image = VK_NULL_HANDLE;
        VkImageView colorView = VK_NULL_HANDLE; //render into this
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;

        // depth
        VkImage depthImage = VK_NULL_HANDLE;
        VkImageView depthView = VK_NULL_HANDLE;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;

        VkClearValue clearColor{};
    };
} // namespace chai