#include "VulkanRenderer.h"

#include "../core/VkCheck.h"
#include "../pipeline/PipelineBuilder.h"
#include "../pipeline/ShaderModule.h"

#include <AssetCache.h>
#include <Log.h>
#include <MeshAsset.h>
#include <Primitives.h>
#include <SystemPaths.h>
#include <Window/Window.h>
#include <CameraData.h>
#include "../ImageTransition.h"
#include "../TextureFactory.h"

namespace chai::gfx
{
    VulkanRenderer::VulkanRenderer(chai::IWindow& window,
                                   std::shared_ptr<AssetCache<Mesh>> meshCache,
                                   std::shared_ptr<AssetCache<Texture>> texCache,
                                   VulkanContext& context)
        : window_(window), ctx_(context),
          swapchain_(ctx_,
                     [&] {
                         int w, h;
                         window.framebufferSize(w, h);
                         return VkExtent2D{uint32_t(w), uint32_t(h)};
                     }()),
          meshCache_(meshCache), texCache_(texCache)
    {
        // init vulkan context
        init();
        CHAI_LOG_INFO("VulkanRenderer initialized");
    }

    VulkanRenderer::~VulkanRenderer()
    {
        waitIdle();

        for (int i = 0; i < kFramesInFlight; i++) {
            vmaDestroyBuffer(ctx_.allocator(), frames_[i].cameraBuffer, frames_[i].cameraAlloc);
            vkDestroyFence(ctx_.device(), frames_[i].inFlight, nullptr);
            vkDestroySemaphore(ctx_.device(), frames_[i].imageAvailable, nullptr);
        }

        vkDestroyPipelineLayout(ctx_.device(), pipelineLayout_, nullptr);
        vkDestroyPipeline(ctx_.device(), pipeline_, nullptr);

        // dont need to destory the buffers individually. Command Pool is enough
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

            VK_CHECK(vkAllocateCommandBuffers(ctx_.device(), &cmdAllocInfo, &frames_[i].cmd));

            VK_CHECK(vkCreateFence(ctx_.device(), &fenceCreateInfo, nullptr, &frames_[i].inFlight));

            VK_CHECK(vkCreateSemaphore(
                ctx_.device(), &semaphoreCreateInfo, nullptr, &frames_[i].imageAvailable));

            //camera
            VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufInfo.size = sizeof(CameraData);
            bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            VmaAllocationCreateInfo aci{};
            aci.usage = VMA_MEMORY_USAGE_AUTO;
            aci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                        VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VmaAllocationInfo allocInfo{};
            VK_CHECK(vmaCreateBuffer(ctx_.allocator(),
                                     &bufInfo,
                                     &aci,
                                     &frames_[i].cameraBuffer,
                                     &frames_[i].cameraAlloc,
                                     &allocInfo));
            frames_[i].cameraMapped = allocInfo.pMappedData;

            VkDescriptorSetLayout camLayout = ctx_.cameraSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &camLayout;
            VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &frames_[i].cameraSet));

            VkDescriptorBufferInfo dbi{};
            dbi.buffer = frames_[i].cameraBuffer;
            dbi.offset = 0;
            dbi.range = sizeof(CameraData);

            VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            write.dstSet = frames_[i].cameraSet;
            write.dstBinding = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &dbi;
            vkUpdateDescriptorSets(ctx_.device(), 1, &write, 0, nullptr);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // we love some temporary code...as long as its actually temporary
        VkPushConstantRange pcRange{};
        pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pcRange.offset = 0;
        pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pcRange.size = sizeof(PushConstants);

        VkDescriptorSetLayout setLayouts[] = {ctx_.cameraSetLayout(), ctx_.materialSetLayout()};
        VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pcRange;
        layoutInfo.setLayoutCount = 2;
        layoutInfo.pSetLayouts = setLayouts;

        VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &layoutInfo, nullptr, &pipelineLayout_));

        const auto shaderDir = executableDir() / "shaders";
        VkShaderModule vert = loadShaderModule(ctx_.device(), shaderDir / "triangle.vert.spv");
        VkShaderModule frag = loadShaderModule(ctx_.device(), shaderDir / "triangle.frag.spv");
        if (vert == VK_NULL_HANDLE || frag == VK_NULL_HANDLE) {
            CHAI_LOG_CRITICAL("Triangle shaders failed to load from {}", shaderDir.string());
            return;
        }

        auto attrs = vertexAttributes();
        auto bind = vertexBinding();

        pipeline_ = PipelineBuilder{}
                        .setShaders(vert, frag)
                        .setVertexInput({attrs.begin(), attrs.end()}, bind)
                        .setColorFormat(swapchain_.format())
                        .enableDepthTest()
                        .setDepthFormat(swapchain_.depthFormat())
                        .disableBlending()
                        .build(ctx_.device(), pipelineLayout_);

        vkDestroyShaderModule(ctx_.device(), vert, nullptr);
        vkDestroyShaderModule(ctx_.device(), frag, nullptr);
    }

    void VulkanRenderer::renderFrame(const FrameRenderData& renderData)
    {
        meshCache_->tick();
        if (needsResize_)
            recreateSwapchain();

        FrameData& frame = frames_[currentFrame_];

        // Wait until this frame slots previous work is done.
        VK_CHECK(vkWaitForFences(ctx_.device(), 1, &frame.inFlight, VK_TRUE, UINT64_MAX));

        CameraData camUBO{};
        camUBO.view = renderData.views[0].view;
        camUBO.proj = renderData.views[0].proj;
        camUBO.viewProj = renderData.views[0].proj * renderData.views[0].view;
        camUBO.position = math::Vec3{0, 0, 0};
        std::memcpy(frame.cameraMapped, &camUBO, sizeof(camUBO));

        RenderTargetView view{};
        uint32_t imageIndex = 0;
        if (!swapchain_.acquireNext(frame.imageAvailable, view, imageIndex)) {
            recreateSwapchain();
            return;
        }

        //debugging
        view.clearColor = {{{0.05f, 0.10f, 0.15f, 1.0f}}};

        VK_CHECK(vkResetFences(ctx_.device(), 1, &frame.inFlight));

        VkCommandBuffer cmd = frame.cmd;
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

        transitionImage(cmd, view.image, ImageState::Undefined, ImageState::ColorAttachment);
        transitionImage(cmd,
                        view.depthImage,
                        ImageState::Undefined,
                        ImageState::DepthAttachment);

        VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        color.imageView = view.colorView;
        color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.clearValue = view.clearColor;

        VkRenderingAttachmentInfo depth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depth.imageView = view.depthView; // from RenderTargetView (plumbed through last turn)
        depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.storeOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE; // depth is scratch, not needed after frame
        depth.clearValue.depthStencil = {1.0f, 0};

        VkRenderingInfo rendering{VK_STRUCTURE_TYPE_RENDERING_INFO};
        rendering.renderArea = VkRect2D{{0, 0}, view.extent};
        rendering.layerCount = 1;
        rendering.colorAttachmentCount = 1;
        rendering.pColorAttachments = &color;
        rendering.pDepthAttachment = &depth;

        // DRAW
        vkCmdBeginRendering(cmd, &rendering);
        renderScene(cmd, view, renderData);
        vkCmdEndRendering(cmd);

        transitionImage(cmd, view.image, ImageState::ColorAttachment, ImageState::Present);

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

    void VulkanRenderer::renderScene(VkCommandBuffer cmd,
                                     const RenderTargetView& view,
                                     const FrameRenderData& renderData)
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

        VkViewport viewport{0, 0, float(view.extent.width), float(view.extent.height), 0.f, 1.f};
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        VkRect2D scissor{{0, 0}, view.extent};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        FrameData& frame = frames_[currentFrame_];
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                0,
                                1,
                                &frame.cameraSet,
                                0,
                                nullptr);

        for (const RenderView& rv : renderData.views) {
            //bindCameraUBO(rv);
            for (const RenderItem& item : renderData.items) {
                //check frustum culling?
                {
                    const GpuMesh* mesh = meshCache_->resource(item.mesh);
                    if (!mesh)
                        continue; // not ready, skip

                    const GpuTexture* tex = texCache_->resource(item.texture);
                    //if (!tex)
                    //    tex = defaultWhite_;
                    vkCmdBindDescriptorSets(cmd,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            pipelineLayout_,
                                            1,
                                            1,
                                            &tex->set,
                                            0,
                                            nullptr);

                    PushConstants consts;
                    consts.model = item.model;
                    consts.color = item.color;
                    vkCmdPushConstants(cmd,
                                       pipelineLayout_,
                                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                       0,
                                       sizeof(PushConstants),
                                       &consts);

                    VkBuffer vb = mesh->vertexBuffer.handle;
                    VkDeviceSize offset = 0;
                    vkCmdBindVertexBuffers(cmd, 0, 1, &vb, &offset);
                    vkCmdBindIndexBuffer(cmd, mesh->indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(cmd, mesh->indexCount, 1, 0, 0, 0);
                }
            }
        }
    }
} // namespace chai::gfx