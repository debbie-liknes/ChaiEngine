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

    //I almost think this wrapper is too simplistic hmm
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
                      VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                      uint32_t layerCount = (~0U),
                      uint32_t mipCount = (~0U));
}