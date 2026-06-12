#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    enum class ImageState {
        Undefined,

        ColorAttachment,
        DepthAttachment,

        ShaderRead,

        TransferSrc,
        TransferDst,

        Present
    };

    struct ImageStateInfo {
        VkImageLayout layout;
        VkPipelineStageFlags2 stage;
        VkAccessFlags2 access;
        VkImageAspectFlags aspect;
    };

    void transitionImage(VkCommandBuffer cmd,
                         VkImage image,
                         ImageState oldState,
                         ImageState newState);

    void imageBarrier(VkCommandBuffer cmd,
                      VkImage image,
                      VkImageLayout oldLayout,
                      VkImageLayout newLayout,
                      VkPipelineStageFlags2 srcStage,
                      VkAccessFlags2 srcAccess,
                      VkPipelineStageFlags2 dstStage,
                      VkAccessFlags2 dstAccess,
                      VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
}