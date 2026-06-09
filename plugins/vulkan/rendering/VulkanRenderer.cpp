#include "VulkanRenderer.h"
#include <Window/Window.h>
#include <Log.h>
#include "../core/VkCheck.h"
#include <SystemPaths.h>
#include "../pipeline/ShaderModule.h"
#include "../pipeline/PipelineBuilder.h"

namespace
{
    //from vk guide but i need to understand this better tbh
    void transitionImage(VkCommandBuffer cmd,
                         VkImage image,
                         VkImageLayout oldLayout,
                         VkImageLayout newLayout)
    {
        VkImageMemoryBarrier2 barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;
        barrier.subresourceRange = VkImageSubresourceRange{
            VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS};

        VkDependencyInfo dep{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
        dep.imageMemoryBarrierCount = 1;
        dep.pImageMemoryBarriers = &barrier;
        vkCmdPipelineBarrier2(cmd, &dep);
    }
} // namespace

namespace chai::gfx
{
    VulkanRenderer::VulkanRenderer(chai::IWindow& window) : 
        window_(window), ctx_(window), 
        swapchain_(ctx_, [&] {
              int w, h;
              window.framebufferSize(w, h);
              return VkExtent2D{uint32_t(w), uint32_t(h)};
          }())
    {
        init();
        CHAI_LOG_INFO("VulkanRenderer initialized");
    }

    VulkanRenderer::~VulkanRenderer()
    {
        waitIdle();
        for (int i = 0; i < kFramesInFlight; i++) {
            vkDestroyFence(ctx_.device(), frames_[i].inFlight, nullptr);
            vkDestroySemaphore(ctx_.device(), frames_[i].imageAvailable, nullptr);
        }

        vkDestroyPipelineLayout(ctx_.device(), pipelineLayout_, nullptr);
        vkDestroyPipeline(ctx_.device(), trianglePipeline_, nullptr);

        //dont need to destory the buffers individually. Command Pool is enough
        vkDestroyCommandPool(ctx_.device(), cmdPool_, nullptr);
        CHAI_LOG_INFO("VulkanRenderer destroyed");
    }

    void VulkanRenderer::init()
    {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext = nullptr;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = ctx_.graphicsFamily();

        VK_CHECK(vkCreateCommandPool(ctx_.device(), &commandPoolInfo, nullptr, &cmdPool_));

        VkFenceCreateInfo fenceCreateInfo = fenceCreate(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphoreCreateInfo = semaphoreCreate();

        for (int i = 0; i < kFramesInFlight; i++) {

            // allocate the default command buffer that we will use for rendering
            VkCommandBufferAllocateInfo cmdAllocInfo = {};
            cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdAllocInfo.pNext = nullptr;
            cmdAllocInfo.commandPool = cmdPool_;
            cmdAllocInfo.commandBufferCount = 1;
            cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            VK_CHECK(vkAllocateCommandBuffers(
                ctx_.device(), &cmdAllocInfo, &frames_[i].cmd));

            VK_CHECK(vkCreateFence(ctx_.device(), &fenceCreateInfo, nullptr, &frames_[i].inFlight));

            VK_CHECK(vkCreateSemaphore(
                ctx_.device(), &semaphoreCreateInfo, nullptr, &frames_[i].imageAvailable));

        }

        //////////////////////////////////////////////////////////////////////////////////////
        const auto shaderDir = executableDir() / "shaders";
        VkShaderModule vert = loadShaderModule(ctx_.device(), shaderDir / "triangle.vert.spv");
        VkShaderModule frag = loadShaderModule(ctx_.device(), shaderDir / "triangle.frag.spv");
        if (vert == VK_NULL_HANDLE || frag == VK_NULL_HANDLE) {
            CHAI_LOG_CRITICAL("Triangle shaders failed to load from {}", shaderDir.string());
            return;
        }

        VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &layoutInfo, nullptr, &pipelineLayout_));

        trianglePipeline_ =
            PipelineBuilder{}
                .setShaders(vert, frag)
                .setColorFormat(swapchain_.format())
                .disableDepthTest()
                .disableBlending()
                .build(ctx_.device(), pipelineLayout_);

        vkDestroyShaderModule(ctx_.device(), vert, nullptr);
        vkDestroyShaderModule(ctx_.device(), frag, nullptr);
    }

    void VulkanRenderer::renderFrame()
    {
        if (needsResize_)
            recreateSwapchain();

        FrameData& frame = frames_[currentFrame_];

        // Wait until this frame slot's previous work is done.
        VK_CHECK(vkWaitForFences(ctx_.device(), 1, &frame.inFlight, VK_TRUE, UINT64_MAX));

        RenderTargetView view{};
        uint32_t imageIndex = 0;
        if (!swapchain_.acquireNext(frame.imageAvailable, view, imageIndex)) {
            recreateSwapchain();
            return;
        }
        view.clearColor = {{{0.05f, 0.10f, 0.15f, 1.0f}}}; // cornflower-ish

        VK_CHECK(vkResetFences(ctx_.device(), 1, &frame.inFlight));

        VkCommandBuffer cmd = frame.cmd;
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

        //??
        transitionImage(
            cmd, view.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        color.imageView = view.colorView;
        color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.clearValue = view.clearColor;

        VkRenderingInfo rendering{VK_STRUCTURE_TYPE_RENDERING_INFO};
        rendering.renderArea = VkRect2D{{0, 0}, view.extent};
        rendering.layerCount = 1;
        rendering.colorAttachmentCount = 1;
        rendering.pColorAttachments = &color;

        //DRAW
        vkCmdBeginRendering(cmd, &rendering);
        renderScene(cmd, view);
        vkCmdEndRendering(cmd);

        //??
        transitionImage(cmd,
                        view.image,
                        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(cmd));

        // Need to understand this better
        VkCommandBufferSubmitInfo cmdInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
        cmdInfo.commandBuffer = cmd;

        VkSemaphoreSubmitInfo wait{VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
        wait.semaphore = frame.imageAvailable;
        wait.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSemaphoreSubmitInfo signal{VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
        signal.semaphore = swapchain_.renderFinished(imageIndex);
        signal.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;

        VkSubmitInfo2 submit{VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
        submit.waitSemaphoreInfoCount = 1;
        submit.pWaitSemaphoreInfos = &wait;
        submit.signalSemaphoreInfoCount = 1;
        submit.pSignalSemaphoreInfos = &signal;
        submit.commandBufferInfoCount = 1;
        submit.pCommandBufferInfos = &cmdInfo;

        
        VK_CHECK(vkQueueSubmit2(ctx_.graphicsQueue(), 1, &submit, frame.inFlight));

        if (!swapchain_.present(imageIndex))
            needsResize_ = true;

        currentFrame_ = (currentFrame_ + 1) % kFramesInFlight;
    }

    void VulkanRenderer::onResize(int width, int height)
    {
        needsResize_ = true;
    }

    void VulkanRenderer::waitIdle()
    {
        vkDeviceWaitIdle(ctx_.device());
    }

    VkFenceCreateInfo VulkanRenderer::fenceCreate(VkFenceCreateFlags flags)
    {
        VkFenceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = flags;

        return info;
    }

    VkSemaphoreCreateInfo VulkanRenderer::semaphoreCreate(VkSemaphoreCreateFlags flags)
    {
        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        return info;
    }

    void VulkanRenderer::recreateSwapchain()
    {
        int w = 0, h = 0;
        window_.framebufferSize(w, h);
        if (w == 0 || h == 0)
            return; // minimized? skip until restored

        waitIdle();
        swapchain_.recreate(VkExtent2D{uint32_t(w), uint32_t(h)});
        needsResize_ = false;
    }

    void VulkanRenderer::renderScene(VkCommandBuffer cmd, const RenderTargetView& view)
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline_);

        VkViewport vp{0, 0, float(view.extent.width), float(view.extent.height), 0.0f, 1.0f};
        vkCmdSetViewport(cmd, 0, 1, &vp);
        VkRect2D scissor{{0, 0}, view.extent};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdDraw(cmd, 3, 1, 0, 0); 
    }

}