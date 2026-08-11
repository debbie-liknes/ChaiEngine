/**
 * @file ImageTransition.h
 */
#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    enum class ImageState {
        Undefined,

        ColorAttachment,
        DepthAttachment,

        ShaderRead,
        DepthShaderRead,

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
                         ImageState newState,
                         uint32_t baseMip = 0,
                         uint32_t mipLevel = 1);

    void imageBarrier(VkCommandBuffer cmd,
                      VkImage image,
                      VkImageLayout oldLayout,
                      VkImageLayout newLayout,
                      VkPipelineStageFlags2 srcStage,
                      VkAccessFlags2 srcAccess,
                      VkPipelineStageFlags2 dstStage,
                      VkAccessFlags2 dstAccess,
                      VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                      uint32_t layerCount = (~0U),
                      uint32_t mipCount = 1,
                      uint32_t baseMip = 0);
} // namespace chai::gfx