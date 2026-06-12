#include "ImageTransition.h"
#include <cstdlib>

namespace chai::gfx
{
    static ImageStateInfo getStateInfo(ImageState state)
    {
        switch (state) {
            case ImageState::Undefined:
                return {VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_PIPELINE_STAGE_2_NONE,
                        0,
                        VK_IMAGE_ASPECT_COLOR_BIT};

            case ImageState::ColorAttachment:
                return {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                        VK_IMAGE_ASPECT_COLOR_BIT};

            case ImageState::DepthAttachment:
                return {VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                        VK_IMAGE_ASPECT_DEPTH_BIT};

            case ImageState::ShaderRead:
                return {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                        VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                        VK_IMAGE_ASPECT_COLOR_BIT};

            case ImageState::TransferSrc:
                return {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        VK_IMAGE_ASPECT_COLOR_BIT};

            case ImageState::TransferDst:
                return {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_WRITE_BIT,
                        VK_IMAGE_ASPECT_COLOR_BIT};

            case ImageState::Present:
                return {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                        VK_PIPELINE_STAGE_2_NONE,
                        0,
                        VK_IMAGE_ASPECT_COLOR_BIT};
        }

        return {VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_NONE, 0, VK_IMAGE_ASPECT_COLOR_BIT};
    }

    void imageBarrier(VkCommandBuffer cmd,
                      VkImage image,
                      VkImageLayout oldLayout,
                      VkImageLayout newLayout,
                      VkPipelineStageFlags2 srcStage,
                      VkAccessFlags2 srcAccess,
                      VkPipelineStageFlags2 dstStage,
                      VkAccessFlags2 dstAccess,
                      VkImageAspectFlags aspect)
    {
        VkImageMemoryBarrier2 barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};

        barrier.srcStageMask = srcStage;
        barrier.srcAccessMask = srcAccess;

        barrier.dstStageMask = dstStage;
        barrier.dstAccessMask = dstAccess;

        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.image = image;

        barrier.subresourceRange = {
            aspect, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS};

        VkDependencyInfo dep{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};

        dep.imageMemoryBarrierCount = 1;
        dep.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(cmd, &dep);
    }

    void transitionImage(VkCommandBuffer cmd, VkImage image, ImageState oldState, ImageState newState)
    {
        const auto src = getStateInfo(oldState);
        const auto dst = getStateInfo(newState);

        imageBarrier(cmd,
                     image,
                     src.layout,
                     dst.layout,
                     src.stage,
                     src.access,
                     dst.stage,
                     dst.access,
                     dst.aspect);
    }
}