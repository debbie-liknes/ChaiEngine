#include "Swapchain.h"
#include "VulkanContext.h"
#include "core/VkCheck.h"

namespace chai::gfx
{
    Swapchain::Swapchain(VulkanContext& ctx, VkExtent2D extent) : ctx_(ctx)
    {
        build(extent);
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }

    void Swapchain::build(VkExtent2D extent)
    {
        vkb::SwapchainBuilder swapchainBuilder{ctx_.physicalDevice(), ctx_.device(), ctx_.surface()};

        format_ = VK_FORMAT_B8G8R8A8_UNORM;

        vkbSwapchain_ = swapchainBuilder
                            //.use_default_format_selection()
                            .set_desired_format(VkSurfaceFormatKHR{
                                .format = format_, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                            // use vsync present mode
                            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                            .set_desired_extent(extent.width, extent.height)
                            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                            .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                            .build()
                            .value();

        extent_ = vkbSwapchain_.extent;
        // store swapchain and its related images
        swapchain_ = vkbSwapchain_.swapchain;
        images_ = vkbSwapchain_.get_images().value();
        views_ = vkbSwapchain_.get_image_views().value();

        VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        renderFinished_.resize(images_.size());
        for (auto& s : renderFinished_)
            VK_CHECK(vkCreateSemaphore(ctx_.device(), &semInfo, nullptr, &s));
    }

    void Swapchain::destroy()
    {
        for (auto s : renderFinished_)
            vkDestroySemaphore(ctx_.device(), s, nullptr);
        renderFinished_.clear();

        for (auto view : views_)
            vkDestroyImageView(ctx_.device(), view, nullptr);
        views_.clear();

        vkDestroySwapchainKHR(ctx_.device(), swapchain_, nullptr);
        swapchain_ = VK_NULL_HANDLE;
    }

    bool Swapchain::acquireNext(VkSemaphore imageAvailable,
                                RenderTargetView& outView,
                                uint32_t& outImageIndex)
    {
        VkResult r = vkAcquireNextImageKHR(
            ctx_.device(), swapchain_, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &outImageIndex);

        if (r == VK_ERROR_OUT_OF_DATE_KHR) // window resized — caller rebuilds
            return false;
        if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) // suboptimal is still usable
            VK_CHECK(r);                               // genuine failure: abort

        outView = RenderTargetView{
            .extent = extent_,
            .image = images_[outImageIndex], // needed for layout barriers
            .colorView = views_[outImageIndex],
            .colorFormat = format_,
            .clearColor = {} // renderFrame sets the color
        };
        return true;
    }

    bool Swapchain::present(uint32_t imageIndex)
    {
        VkSemaphore wait = renderFinished_[imageIndex];
        VkPresentInfoKHR info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &wait;
        info.swapchainCount = 1;
        info.pSwapchains = &swapchain_;
        info.pImageIndices = &imageIndex;

        VkResult r = vkQueuePresentKHR(ctx_.presentQueue(), &info);
        if (r == VK_ERROR_OUT_OF_DATE_KHR || r == VK_SUBOPTIMAL_KHR)
            return false;
        VK_CHECK(r);
        return true;
    }

    void Swapchain::recreate(VkExtent2D extent)
    {
        destroy();
        build(extent);
    }
}