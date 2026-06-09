#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "RenderTargetView.h"
#include "VkBootstrap.h"

namespace chai::gfx
{
    class VulkanContext;

    // The swapchain is NOT a render target — it PRODUCES one per frame (the
    // acquired image) and PRESENTS it afterward. That's the "render to a target"
    // vs "present a target to a window" split, made concrete: acquireNext()
    // gives you a RenderTargetView; present() shows it.
    class Swapchain
    {
    public:
        Swapchain(VulkanContext& ctx, VkExtent2D extent);
        ~Swapchain();

        // Acquire the next image. Returns false if the swapchain is out of date
        // (window resized / minimized) — caller should recreate() and skip the
        // frame. On success, `outView` describes the acquired image and
        // `outImageIndex` is needed for present().
        bool
        acquireNext(VkSemaphore imageAvailable, RenderTargetView& outView, uint32_t& outImageIndex);

        // Present a previously-acquired image, waiting on renderFinished.
        // Returns false if the swapchain became out of date during present.
        bool present(uint32_t imageIndex);
        VkSemaphore renderFinished(uint32_t imageIndex) const
        {
            return renderFinished_[imageIndex];
        }

        // Tear down and rebuild at a new size (window resize). Caller must have
        // waited for the device to be idle first.
        void recreate(VkExtent2D extent);

        VkFormat format() const { return format_; }
        VkExtent2D extent() const { return extent_; }

    private:
        void build(VkExtent2D extent);
        void destroy();

        vkb::Swapchain vkbSwapchain_;
        VulkanContext& ctx_;
        VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
        VkFormat format_{};
        VkExtent2D extent_{};
        std::vector<VkImage> images_;    // owned by the swapchain, not by us
        std::vector<VkImageView> views_; // we create/destroy these
        std::vector<VkSemaphore> renderFinished_;
    };
} // namespace chai