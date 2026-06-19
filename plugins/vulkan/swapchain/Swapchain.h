/**
 * @file Swapchain.h
 */
#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "../resources/RenderTargetView.h"
#include "VkBootstrap.h"
#include <vk_mem_alloc.h>

namespace chai::gfx
{
    class VulkanContext;

    /**
     * @brief Produces one render target view per frame
     */
    class Swapchain
    {
    public:
        Swapchain(VulkanContext& ctx, VkExtent2D extent);
        ~Swapchain();

        /**
         * @brief Acquires the next image
         * @return false if the swapchain is out of date
         */
        bool
        acquireNext(VkSemaphore imageAvailable, RenderTargetView& outView, uint32_t& outImageIndex);

        /**
         * @brief Present an image acquired with acquireNext()
         * @return false if the swapchain is out of date
         */
        bool present(uint32_t imageIndex);


        VkSemaphore renderFinished(uint32_t imageIndex) const
        {
            return renderFinished_[imageIndex];
        }

        /**
         * @brief Tears down and recreates the swap chain. Done on resize
         */
        void recreate(VkExtent2D extent);

        VkFormat format() const { return format_; }
        VkFormat depthFormat() const { return depthFormat_; }
        VkExtent2D extent() const { return extent_; }

    private:
        void build(VkExtent2D extent);
        void destroy();

        vkb::Swapchain vkbSwapchain_;
        VulkanContext& ctx_;
        VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
        VkFormat format_{};
        VkExtent2D extent_{};
        std::vector<VkImage> images_;
        std::vector<VkImageView> views_;
        std::vector<VkSemaphore> renderFinished_;

        VkImage depthImage_;
        VkImageView depthView_;
        VkFormat depthFormat_;
        VmaAllocation depthAlloc_;
    };
} // namespace chai