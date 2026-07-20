/**
 * @file RenderTargetView.h
 * @brief Description of where to draw this frame.
 */
#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <imgui.h>
#include "../renderer/VulkanContext.h"

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

    /**
     * @brief Support multi viewports
     */
    struct ViewportTarget 
    {
        RenderTargetView view;

        VmaAllocation colorAlloc = VK_NULL_HANDLE;
        VmaAllocation depthAlloc = VK_NULL_HANDLE;

        //TODO: another place where ImGui is sneaking in
        ImTextureID imguiTextureId = 0;

        void destroy(VulkanContext& ctx)
        {
            vmaDestroyImage(ctx.allocator(), view.image, colorAlloc);
            vmaDestroyImage(ctx.allocator(), view.depthImage, depthAlloc);

            vkDestroyImageView(ctx.device(), view.colorView, nullptr);
            vkDestroyImageView(ctx.device(), view.depthView, nullptr);
        }
    };
} // namespace chai