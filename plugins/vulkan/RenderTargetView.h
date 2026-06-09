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

        VkImage image = VK_NULL_HANDLE;         // for layout barriers
        VkImageView colorView = VK_NULL_HANDLE; //render into this
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkClearValue clearColor{}; // loadOp = CLEAR uses this

        //Later issues
        // VkImageView depthView = VK_NULL_HANDLE;
        // VkFormat    depthFormat = VK_FORMAT_UNDEFINED;
    };
} // namespace chai