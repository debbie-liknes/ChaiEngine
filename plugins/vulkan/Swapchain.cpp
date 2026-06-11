#include "Swapchain.h"
#include "VulkanContext.h"
#include "core/VkCheck.h"

namespace chai::gfx
{
    Swapchain::Swapchain(VulkanContext& ctx, VkExtent2D extent)
        : ctx_(ctx), depthFormat_(VK_FORMAT_D32_SFLOAT)
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
                            .set_desired_format(VkSurfaceFormatKHR{
                                .format = format_, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
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

        // depth
        VkImageCreateInfo imgInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = depthFormat_;
        imgInfo.extent = {extent_.width, extent_.height, 1};
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VmaAllocationCreateInfo allocCI{};
        allocCI.usage = VMA_MEMORY_USAGE_AUTO;
        allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        VK_CHECK(vmaCreateImage(
            ctx_.allocator(), &imgInfo, &allocCI, &depthImage_, &depthAlloc_, nullptr));

        VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        viewInfo.image = depthImage_;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat_;
        viewInfo.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        VK_CHECK(vkCreateImageView(ctx_.device(), &viewInfo, nullptr, &depthView_));

        //per image semaphore
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

        vkDestroyImageView(ctx_.device(), depthView_, nullptr);
        vmaDestroyImage(ctx_.allocator(), depthImage_, depthAlloc_);
        depthView_ = VK_NULL_HANDLE;
        depthImage_ = VK_NULL_HANDLE;

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

        if (r == VK_ERROR_OUT_OF_DATE_KHR) // window resized
            return false;
        if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) // suboptimal? TODO: can i do something about that?
            VK_CHECK(r);

        outView = RenderTargetView{
            .extent = extent_,
            .image = images_[outImageIndex],
            .colorView = views_[outImageIndex],
            .colorFormat = format_,
            .depthImage = depthImage_,
            .depthView = depthView_,
            .depthFormat = depthFormat_,
            .clearColor = {}
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