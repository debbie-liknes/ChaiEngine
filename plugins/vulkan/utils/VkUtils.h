#pragma once
#include <vulkan/vulkan.h>
#include "VkCheck.h"
#include "../resources/VulkanTexture.h"
#include <Assets/TextureFormat.h>
#include <functional>
#include "../renderer/VulkanContext.h"

namespace chai::gfx
{
    inline VkFormat convertTextureFormat(const TextureFormat& format)
    {
        switch (format) {
            case TextureFormat::RGBA8_SRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::RGBA8_UNORM:
                return VK_FORMAT_R8G8B8A8_UNORM;
        }

        return VK_FORMAT_UNDEFINED;
    }

    inline void immediateSubmit(VulkanContext& ctx, std::function<void(VkCommandBuffer)>&& fn)
    {
        const VkFence fence = ctx.immediateFence();
        const VkCommandBuffer cmd = ctx.immediateCmd();

        VK_CHECK(vkResetFences(ctx.device(), 1, &fence));
        VK_CHECK(vkResetCommandBuffer(cmd, 0));

        VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

        fn(cmd); // caller's commands

        VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;

        // Submit to the graphics queue
        VK_CHECK(vkQueueSubmit(ctx.graphicsQueue(), 1, &submit, fence));
        VK_CHECK(vkWaitForFences(ctx.device(), 1, &fence, VK_TRUE, UINT64_MAX)); // <-- the block
    }

    inline void generateMipmaps(
        VkCommandBuffer cmd, VkImage image, int32_t width, int32_t height, uint32_t mipLevels)
    {
        auto barrier = [&](uint32_t level,
                           VkImageLayout oldL,
                           VkImageLayout newL,
                           VkAccessFlags2 srcA,
                           VkAccessFlags2 dstA) {
            VkImageMemoryBarrier2 b{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
            b.image = image;
            b.oldLayout = oldL;
            b.newLayout = newL;
            b.srcAccessMask = srcA;
            b.dstAccessMask = dstA;
            b.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            b.dstStageMask = (newL == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                                 ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
                                 : VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            b.srcQueueFamilyIndex = b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, level, 1, 0, 1};
            VkDependencyInfo dep{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
            dep.imageMemoryBarrierCount = 1;
            dep.pImageMemoryBarriers = &b;
            vkCmdPipelineBarrier2(cmd, &dep);
        };

        int32_t mw = width, mh = height;
        for (uint32_t i = 1; i < mipLevels; ++i) {
            // previous level: written -> readable as blit source
            barrier(i - 1,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    VK_ACCESS_2_TRANSFER_READ_BIT);

            VkImageBlit2 blit{VK_STRUCTURE_TYPE_IMAGE_BLIT_2};
            blit.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, 1};
            blit.srcOffsets[1] = {mw, mh, 1};
            blit.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1};
            blit.dstOffsets[1] = {mw > 1 ? mw / 2 : 1, mh > 1 ? mh / 2 : 1, 1};

            VkBlitImageInfo2 bi{VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2};
            bi.srcImage = image;
            bi.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            bi.dstImage = image;
            bi.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            bi.regionCount = 1;
            bi.pRegions = &blit;
            bi.filter = VK_FILTER_LINEAR;
            vkCmdBlitImage2(cmd, &bi);

            // previous level done -> shader read
            barrier(i - 1,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_ACCESS_2_TRANSFER_READ_BIT,
                    VK_ACCESS_2_SHADER_READ_BIT);

            if (mw > 1)
                mw /= 2;
            if (mh > 1)
                mh /= 2;
        }
        
        barrier(mipLevels - 1,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_2_TRANSFER_WRITE_BIT,
                VK_ACCESS_2_SHADER_READ_BIT);
    }
} // namespace chai::gfx