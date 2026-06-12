#pragma once
#include <vulkan/vulkan.h>
#include <Loaders/ITextureLoader.h>
#include "core/VkCheck.h"

namespace chai::gfx
{
    inline VkFormat convertTextureFormat(const TextureFormat& format) {
        switch (format) {
            case TextureFormat::RGBA8_SRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::RGBA8_UNORM:
                return VK_FORMAT_R8G8B8A8_UNORM;
        }

        return VK_FORMAT_UNDEFINED;
    }

    //temp
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

        // Submit to the graphics queue for now
        // TODO: transfer queue?
        VK_CHECK(vkQueueSubmit(ctx.graphicsQueue(), 1, &submit, fence));
        VK_CHECK(vkWaitForFences(ctx.device(), 1, &fence, VK_TRUE, UINT64_MAX)); // <-- the block
    }
}