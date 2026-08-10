#include "VulkanRenderer.h"

#include "../commands/ImageTransition.h"
#include "../pipeline/PipelineBuilder.h"
#include "../pipeline/PipelineHelpers.h"
#include "../pipeline/ShaderModule.h"
#include "../resources/TextureFactory.h"
#include "../resources/VulkanVertex.h"
#include "../utils/VkCheck.h"

#include <AssetCache.h>
#include <Assets/DefaultTextures.h>
#include <Assets/MeshAsset.h>
#include <Runtime/SystemPaths.h>
#include <Log.h>
#include <Rendering/CameraData.h>
#include <Window/Window.h>
#include <numeric>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <Input/IInput.h>
#include <tracy/Tracy.hpp>

namespace chai::gfx
{
    const uint32_t kIrradianceSize = 32;
    const uint32_t kPrefilterSize = 128;
    const uint32_t kShadowMapSize = 1024;
    const uint32_t kBRDFLUT = 512;

    VulkanRenderer::VulkanRenderer(chai::IWindow& window,
                                   std::shared_ptr<AssetCache<Mesh>> meshCache,
                                   std::shared_ptr<AssetCache<Texture>> texCache,
                                   std::shared_ptr<AssetCache<Material>> matCache,
                                   std::shared_ptr<ModelRegistry> texReg,
                                   VulkanContext& context,
                                   chai::ServiceLocator& locator)
        : window_(window), ctx_(context),
          swapchain_(ctx_,
                     [&] {
                         int w, h;
                         window.framebufferSize(w, h);
                         return VkExtent2D{uint32_t(w), uint32_t(h)};
                     }()),
          locator_(locator), meshCache_(meshCache), texCache_(texCache), materialCache_(matCache),
          modelReg_(texReg)
    {
        init();
        CHAI_LOG_INFO("VulkanRenderer initialized");
    }

    VulkanRenderer::~VulkanRenderer()
    {
        waitIdle();

        profiler_.shutdown(ctx_.device());

        for (auto& slot : viewportSlots_) {
            for (int i = 0; i < kFramesInFlight; i++) {
                vmaDestroyBuffer(
                    ctx_.allocator(), slot.viewport.cameraBuffer[i], slot.viewport.cameraAlloc[i]);
                slot.viewport.targets[i].destroy(ctx_);
            }
        }

        vkDestroySampler(ctx_.device(), linearSampler_, nullptr);

        for (int i = 0; i < kFramesInFlight; i++) {
            vmaDestroyBuffer(ctx_.allocator(), frames_[i].lightBuffer, frames_[i].lightAlloc);
            vkDestroyFence(ctx_.device(), frames_[i].inFlight, nullptr);
            vkDestroySemaphore(ctx_.device(), frames_[i].imageAvailable, nullptr);
            frames_[i].shadowTarget.destroy(ctx_);
        }

        vkDestroyPipeline(ctx_.device(), pbrPipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), pbrBlendPipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), skyboxPipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), irradiancePipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), brdfLutPipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), prefilterPipeline_, nullptr);
        vkDestroyPipeline(ctx_.device(), shadowPipeline_, nullptr);

        vkDestroyPipelineLayout(ctx_.device(), pipelineLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), irradianceLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), brdfLutLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), prefilterLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), shadowLayout_, nullptr);

        irradianceTarget_.destroy(ctx_);
        brdfLut_.destroy(ctx_);
        prefilterTarget_.destroy(ctx_);

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

        VkSamplerCreateInfo samplerInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        VK_CHECK(vkCreateSampler(ctx_.device(), &samplerInfo, nullptr, &linearSampler_));

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

            frames_[i].shadowTarget =
                createDepth2D(ctx_, kShadowMapSize, kShadowMapSize, VK_FORMAT_D32_SFLOAT, true);

            // lights
            {
                VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
                bufInfo.size = sizeof(LightData);
                bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

                VmaAllocationCreateInfo aci{};
                aci.usage = VMA_MEMORY_USAGE_AUTO;
                aci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                            VMA_ALLOCATION_CREATE_MAPPED_BIT;

                VmaAllocationInfo allocInfo{};
                VK_CHECK(vmaCreateBuffer(ctx_.allocator(),
                                         &bufInfo,
                                         &aci,
                                         &frames_[i].lightBuffer,
                                         &frames_[i].lightAlloc,
                                         &allocInfo));
                frames_[i].lightMapped = allocInfo.pMappedData;

                VkDescriptorSetLayout lightLayout = ctx_.lightSetLayout();
                VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
                dsai.descriptorPool = ctx_.descriptorPool();
                dsai.descriptorSetCount = 1;
                dsai.pSetLayouts = &lightLayout;
                VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &frames_[i].lightSet));

                VkDescriptorBufferInfo dbi{};
                dbi.buffer = frames_[i].lightBuffer;
                dbi.offset = 0;
                dbi.range = sizeof(LightData);

                VkWriteDescriptorSet writes[2]{};
                writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[0].dstSet = frames_[i].lightSet;
                writes[0].dstBinding = 0;
                writes[0].descriptorCount = 1;
                writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writes[0].pBufferInfo = &dbi;

                VkDescriptorImageInfo imageInfo{};
                imageInfo.sampler = frames_[i].shadowTarget.sampler;
                imageInfo.imageView = frames_[i].shadowTarget.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[1].dstSet = frames_[i].lightSet;
                writes[1].dstBinding = 1;
                writes[1].descriptorCount = 1;
                writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writes[1].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(ctx_.device(), 2, writes, 0, nullptr);
            }
        }

        profiler_.initialize(ctx_.device(), ctx_.physicalDevice(), kFramesInFlight);

        // only need this one, because its environment
        irradianceTarget_ = createCube(ctx_, kIrradianceSize, VK_FORMAT_R16G16B16A16_SFLOAT, 1);
        brdfLut_ = createColor2D(ctx_, kBRDFLUT, kBRDFLUT, VK_FORMAT_R16G16B16A16_SFLOAT);
        prefilterTarget_ = createCube(ctx_, kPrefilterSize, VK_FORMAT_R16G16B16A16_SFLOAT, 5);

        setupPipelines();

        if (!brdfBaked_) {
            bakeBrdfLut();
            brdfBaked_ = true;
        }

        // skybox
        if (environmentSet_ == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.environmentSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &environmentSet_));
        }

        auto cube = texCache_->ingest(makeAssetId("builtin:cube"), createDefaultCubeTexture());
        // we require the default textures to be ready
        ctx_.uploadContext().waitFor(ctx_.uploadContext().lastSubmittedValue());

        //TODO: I dont like that theres no way out. Need to rethink how to default bind the environment
        //Or if default binding (to a black skybox cube) makes sense. 
        while (!texCache_->isReady(cube)) {
            texCache_->tick();
        }
        auto skyResource = texCache_->resource(cube);
        bakeIrradiance(*skyResource);
        bakePrefilter(*skyResource);
        writeEnvironmentSet(*skyResource);
        iblBaked_ = true;
    }

    void VulkanRenderer::applyPendingViewportResizes()
    {
        for (auto& slot : viewportSlots_) {
            if (!slot.alive)
                continue;
            auto& viewport = slot.viewport;
            ViewportTarget& target = viewport.targets[currentFrame_];

            if (viewport.pendingExtent.width > 0 && viewport.pendingExtent.height > 0 &&
                (target.view.extent.width != viewport.pendingExtent.width ||
                 target.view.extent.height != viewport.pendingExtent.height)) {
                recreateViewportTarget(viewport, currentFrame_);
            }
        }
    }

    void VulkanRenderer::startFrame()
    {
        FrameData& frame = frames_[currentFrame_];

        // Wait until this frame slots previous work is done.
        VK_CHECK(vkWaitForFences(ctx_.device(), 1, &frame.inFlight, VK_TRUE, UINT64_MAX));

        applyPendingViewportResizes();

        //sync input to imgui
        ImGuiIO& io = ImGui::GetIO();

        auto input = locator_.tryResolve<IInput>();
        if (input) {
            auto mousePos = input->mousePosition();
            io.AddMousePosEvent(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            io.AddMouseButtonEvent(ImGuiMouseButton_Left, input->mouseDown(MouseButton::Left));
            io.AddMouseButtonEvent(ImGuiMouseButton_Right, input->mouseDown(MouseButton::Right));
            io.AddMouseButtonEvent(ImGuiMouseButton_Middle, input->mouseDown(MouseButton::Middle));

            io.AddMouseWheelEvent(static_cast<float>(input->scrollDelta().x),
                                  static_cast<float>(input->scrollDelta().y));

            io.AddKeyEvent(ImGuiKey_Escape, input->keyDown(Key::Escape));
            io.AddKeyEvent(ImGuiKey_A, input->keyDown(Key::A));
            io.AddKeyEvent(ImGuiKey_B, input->keyDown(Key::B));
            io.AddKeyEvent(ImGuiKey_C, input->keyDown(Key::C));
            io.AddKeyEvent(ImGuiKey_D, input->keyDown(Key::D));
            io.AddKeyEvent(ImGuiKey_E, input->keyDown(Key::E));
            io.AddKeyEvent(ImGuiKey_F, input->keyDown(Key::F));
            io.AddKeyEvent(ImGuiKey_G, input->keyDown(Key::G));
            io.AddKeyEvent(ImGuiKey_H, input->keyDown(Key::H));
            io.AddKeyEvent(ImGuiKey_I, input->keyDown(Key::I));
            io.AddKeyEvent(ImGuiKey_J, input->keyDown(Key::J));
            io.AddKeyEvent(ImGuiKey_K, input->keyDown(Key::K));
            io.AddKeyEvent(ImGuiKey_L, input->keyDown(Key::L));
            io.AddKeyEvent(ImGuiKey_M, input->keyDown(Key::M));
            io.AddKeyEvent(ImGuiKey_N, input->keyDown(Key::N));
            io.AddKeyEvent(ImGuiKey_O, input->keyDown(Key::O));
            io.AddKeyEvent(ImGuiKey_P, input->keyDown(Key::P));
            io.AddKeyEvent(ImGuiKey_Q, input->keyDown(Key::Q));
            io.AddKeyEvent(ImGuiKey_R, input->keyDown(Key::R));
            io.AddKeyEvent(ImGuiKey_S, input->keyDown(Key::S));
            io.AddKeyEvent(ImGuiKey_T, input->keyDown(Key::T));
            io.AddKeyEvent(ImGuiKey_U, input->keyDown(Key::U));
            io.AddKeyEvent(ImGuiKey_V, input->keyDown(Key::V));
            io.AddKeyEvent(ImGuiKey_W, input->keyDown(Key::W));
            io.AddKeyEvent(ImGuiKey_X, input->keyDown(Key::X));
            io.AddKeyEvent(ImGuiKey_Y, input->keyDown(Key::Y));
            io.AddKeyEvent(ImGuiKey_Z, input->keyDown(Key::Z));
            io.AddKeyEvent(ImGuiKey_Backspace, input->keyDown(Key::Backspace));

            for (auto c : input->getTypedCharactersThisFrame())
                io.AddInputCharacter(c);
        }

        std::optional<uint32_t> hoveredCameraIndex;
        for (auto& slot : viewportSlots_) {
            if (!slot.alive)
                continue;
            if (slot.viewport.hovered) {
                hoveredCameraIndex = slot.viewport.cameraViewId;
                break;
            }
        }
        if (hoveredCameraIndex.has_value())
            input->setHoveredCamera(hoveredCameraIndex.value());
        else {
            input->setHoveredCamera(-1);
        }

        beginUIFrame();
    }

    void VulkanRenderer::endFrame() 
    {
        //nothing to do for now
    }

    math::Vec2 VulkanRenderer::getViewportExtent(ViewportHandle handle) const
    {
        auto* vp = const_cast<VulkanRenderer*>(this)->getViewport(handle);
        if (!vp)
            return {0, 0};
        auto& t = vp->targets[currentFrame_];
        return {t.view.extent.width, t.view.extent.height};
    }

    void VulkanRenderer::renderFrame(const FrameRenderData& renderData)
    {
        ZoneScoped

        if (needsResize_)
            recreateSwapchain();

        FrameData& frame = frames_[currentFrame_];

        meshCache_->tick();
        texCache_->tick();
        materialCache_->tick();
        modelReg_->tick();

        stats_.clear();

        if (!iblBaked_ || (renderData.environment.skyboxCube != skyboxCube_)) {
            if (const GpuTexture* sky = texCache_->resource(renderData.environment.skyboxCube)) {
                bakeIrradiance(*sky);
                bakePrefilter(*sky);
                writeEnvironmentSet(*sky);
                iblBaked_ = true;
            }
        }

        LightData lightUBO{};
        lightUBO.color = renderData.sun.color;
        lightUBO.direction = renderData.sun.direction;
        lightUBO.proj = renderData.sun.proj;
        lightUBO.view = renderData.sun.view;
        std::memcpy(frame.lightMapped, &lightUBO, sizeof(lightUBO));

        RenderTargetView view{};
        uint32_t imageIndex = 0;
        if (!swapchain_.acquireNext(frame.imageAvailable, view, imageIndex)) {
            recreateSwapchain();
            return;
        }

        VK_CHECK(vkResetFences(ctx_.device(), 1, &frame.inFlight));

        VkCommandBuffer cmd = frame.cmd;
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

        profiler_.beginFrame(frame.cmd, currentFrame_);

        transitionImage(cmd, view.image, ImageState::Undefined, ImageState::ColorAttachment);
        transitionImage(cmd, view.depthImage, ImageState::Undefined, ImageState::DepthAttachment);

        // sort by opaque/blend
        std::vector<uint32_t> order(renderData.items.size());
        std::iota(order.begin(), order.end(), 0u);

        std::sort(order.begin(), order.end(), [&](uint32_t a, uint32_t b) {
            const GpuMaterial* ma = materialCache_->resource(renderData.items[a].material);
            const GpuMaterial* mb = materialCache_->resource(renderData.items[b].material);
            bool aBlend = ma && ma->alphaMode == AlphaMode::Blend;
            bool bBlend = mb && mb->alphaMode == AlphaMode::Blend;
            if (aBlend != bBlend)
                return !aBlend; // opaque first
            return renderData.items[a].material.index < renderData.items[b].material.index;
        });

        // DRAW
        shadowMapping(cmd, order, renderData);

        for (auto& slot : viewportSlots_) {
            if (!slot.alive)
                continue;
            auto& viewport = slot.viewport;
            ViewportTarget& target = viewport.targets[currentFrame_];
            auto viewIt = std::find_if(renderData.views.begin(),
                                               renderData.views.end(),
                [camId = viewport.cameraViewId](const RenderView& data) {
                                           return data.cameraId == camId;
                                               });
            if (viewIt == renderData.views.end()) {
                CHAI_LOG_WARN("Invalid Camera Id for Viewport {}", slot.viewport.id);
                continue;
            }

            const auto& camView = *viewIt;

            float aspect = static_cast<float>(target.view.extent.width) /
                           static_cast<float>(target.view.extent.height);

            math::Mat4 proj = math::perspectiveVK(
                camView.fovYRadians, aspect, camView.nearPlane, camView.farPlane);

            CameraData camUBO{};
            camUBO.view = camView.view;
            camUBO.proj = proj;
            camUBO.viewProj = proj * camView.view;
            camUBO.position = camView.position;
            std::memcpy(viewport.cameraMapped[currentFrame_], &camUBO, sizeof(camUBO));

            VkRenderingAttachmentInfo vpColor{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
            vpColor.imageView = target.view.colorView;
            vpColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            vpColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vpColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            vpColor.clearValue = target.view.clearColor;

            VkRenderingAttachmentInfo vpDepth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
            vpDepth.imageView = target.view.depthView;
            vpDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            vpDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vpDepth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            vpDepth.clearValue.depthStencil = {1.0f, 0};

            VkRenderingInfo vpRendering{VK_STRUCTURE_TYPE_RENDERING_INFO};
            vpRendering.renderArea = VkRect2D{{0, 0}, target.view.extent};
            vpRendering.layerCount = 1;
            vpRendering.colorAttachmentCount = 1;
            vpRendering.pColorAttachments = &vpColor;
            vpRendering.pDepthAttachment = &vpDepth;

            transitionImage(cmd,
                            target.view.image,
                            viewport.everRendered[currentFrame_] ? ImageState::ShaderRead
                                                                 : ImageState::Undefined,
                            ImageState::ColorAttachment);
            transitionImage(
                cmd, target.view.depthImage, ImageState::Undefined, ImageState::DepthAttachment);

            viewport.everRendered[currentFrame_] = true;

            profiler_.beginRegion(cmd, "Main Pass: " + viewport.id);
            vkCmdBeginRendering(cmd, &vpRendering);
            renderScene(cmd, target.view, renderData, order, viewport.cameraSet[currentFrame_]);
            vkCmdEndRendering(cmd);
            profiler_.endRegion(cmd, "Main Pass: " + viewport.id);

            transitionImage(
                cmd, target.view.image, ImageState::ColorAttachment, ImageState::ShaderRead);
        }

        //Render UI
        endUIFrame();
        renderUI(cmd, view.colorView);

        transitionImage(cmd, view.image, ImageState::ColorAttachment, ImageState::Present);

        VK_CHECK(vkEndCommandBuffer(cmd));

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

        profiler_.endFrame(currentFrame_, ctx_.device());
        stats_.gpuTimeMs = profiler_.getTotalFrameTimeMs();
        auto allRegions = profiler_.getAllRegionTimes();
        for (auto region : allRegions) {
            if (region.first == "Main Pass")
                stats_.mainPass.gpuTimeMs = region.second;
            else if (region.first == "Shadow Pass")
                stats_.shadowPass.gpuTimeMs = region.second;
        }

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
                                     const FrameRenderData& renderData,
                                     const std::vector<uint32_t>& order,
                                     VkDescriptorSet cameraSet)
    {
        VkViewport viewport{0, 0, float(view.extent.width), float(view.extent.height), 0.f, 1.f};
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        VkRect2D scissor{{0, 0}, view.extent};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        FrameData& frame = frames_[currentFrame_];

        // Global sets, bound once
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                0,
                                1,
                                &cameraSet,
                                0,
                                nullptr);
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                2,
                                1,
                                &frame.lightSet,
                                0,
                                nullptr);
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                3,
                                1,
                                &environmentSet_,
                                0,
                                nullptr);

        Handle<Material> lastMaterial{};
        for (uint32_t idx : order) {
            const RenderItem& item = renderData.items[idx];

            const GpuMaterial* mat = materialCache_->resource(item.material);
            if (!mat)
                continue; // mat not ready

            const GpuMesh* mesh = meshCache_->resource(item.mesh);
            if (!mesh)
                continue;

            // this is not scalable, will require refactor when there are too many pipelines
            if (mat->alphaMode == AlphaMode::Blend)
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pbrBlendPipeline_);
            else
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline_);

            if (item.material != lastMaterial) {
                vkCmdBindDescriptorSets(cmd,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout_,
                                        1,
                                        1,
                                        &mat->set,
                                        0,
                                        nullptr);
                lastMaterial = item.material;
            }

            PushConstants consts;
            consts.model = item.model;
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
            stats_.mainPass.drawCalls++;
        }

        // skybox render
        const GpuTexture* cubeTex = texCache_->resource(renderData.environment.skyboxCube);
        if (!cubeTex)
            return;
        ensureSkyboxSet(frame, *cubeTex, renderData.environment.skyboxCube);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline_);

        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout_,
                                3,
                                1,
                                &skyboxSet_,
                                0,
                                nullptr);

        vkCmdDraw(cmd, 3, 1, 0, 0);
    }

    void VulkanRenderer::setupPipelines()
    {
        {
            VkPushConstantRange pcRange{};
            pcRange.offset = 0;
            pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            pcRange.size = sizeof(PushConstants);

            // set 0 = camera, set 1 = material, set 2 = light, set 3 = env
            VkDescriptorSetLayout setLayouts[] = {ctx_.cameraSetLayout(),
                                                  ctx_.materialSetLayout(),
                                                  ctx_.lightSetLayout(),
                                                  ctx_.environmentSetLayout()};
            VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            layoutInfo.pushConstantRangeCount = 1;
            layoutInfo.pPushConstantRanges = &pcRange;
            layoutInfo.setLayoutCount = 4;
            layoutInfo.pSetLayouts = setLayouts;

            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &layoutInfo, nullptr, &pipelineLayout_));
        }

        {
            VkPushConstantRange irrPc{};
            irrPc.offset = 0;
            irrPc.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            irrPc.size = sizeof(int);

            VkDescriptorSetLayout irrSets[] = {ctx_.environmentSetLayout()}; // 1 cube sampler
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 1;
            li.pSetLayouts = irrSets;
            li.pushConstantRangeCount = 1;
            li.pPushConstantRanges = &irrPc;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &irradianceLayout_));
        }

        {
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 0;
            li.pushConstantRangeCount = 0;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &brdfLutLayout_));
        }

        {
            VkPushConstantRange prefilterPc{};
            prefilterPc.offset = 0;
            prefilterPc.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            prefilterPc.size = sizeof(int) + sizeof(float);

            VkDescriptorSetLayout prefilterSets[] = {ctx_.prefilterSetLayout()};
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 1;
            li.pSetLayouts = prefilterSets;
            li.pushConstantRangeCount = 1;
            li.pPushConstantRanges = &prefilterPc;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &prefilterLayout_));
        }

        {
            VkPushConstantRange shadowPc{};
            shadowPc.offset = 0;
            shadowPc.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            shadowPc.size = sizeof(math::Mat4) * 3;

            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 0;
            li.pushConstantRangeCount = 1;
            li.pPushConstantRanges = &shadowPc;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &shadowLayout_));
        }

        auto attrs = vertexAttributes();
        auto bind = vertexBinding();

        pbrPipeline_ = loadPipelineByName(
            ctx_, "pbr.vert.spv", "pbr.frag.spv", pipelineLayout_, [&](PipelineBuilder& b) {
                b.setVertexInput({attrs.begin(), attrs.end()}, bind)
                    .setColorFormat(swapchain_.format())
                    .setDepthFormat(swapchain_.depthFormat())
                    .enableDepthTest()
                    .enableDepthWrite()
                    .disableBlending()
                    .setCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });

        pbrBlendPipeline_ = loadPipelineByName(
            ctx_, "pbr.vert.spv", "pbr.frag.spv", pipelineLayout_, [&](PipelineBuilder& b) {
                b.setVertexInput({attrs.begin(), attrs.end()}, bind)
                    .setColorFormat(swapchain_.format())
                    .setDepthFormat(swapchain_.depthFormat())
                    .enableDepthTest()
                    .disableDepthWrite()
                    .enableBlending()
                    .setCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });

        skyboxPipeline_ = loadPipelineByName(
            ctx_, "skybox.vert.spv", "skybox.frag.spv", pipelineLayout_, [&](PipelineBuilder& b) {
                b.setColorFormat(swapchain_.format())
                    .setDepthFormat(swapchain_.depthFormat())
                    .enableDepthTest()
                    .disableDepthWrite()
                    .setDepthOp(VK_COMPARE_OP_LESS_OR_EQUAL)
                    .disableBlending()
                    .setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });

        irradiancePipeline_ = loadPipelineByName(
            ctx_,
            "irradiance.vert.spv",
            "irradiance.frag.spv",
            irradianceLayout_,
            [&](PipelineBuilder& b) {
                b.setColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
                    .disableBlending()
                    .disableDepthTest()
                    .disableDepthWrite()
                    .setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });

        brdfLutPipeline_ = loadPipelineByName(ctx_,
                                              "brdf_lut.vert.spv",
                                              "brdf_lut.frag.spv",
                                              brdfLutLayout_,
                                              [&](PipelineBuilder& b) {
                                                  b.setColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
                                                      .disableBlending()
                                                      .disableDepthTest()
                                                      .disableDepthWrite()
                                                      .setCullMode(VK_CULL_MODE_NONE,
                                                                   VK_FRONT_FACE_COUNTER_CLOCKWISE);
                                              });

        prefilterPipeline_ = loadPipelineByName(
            ctx_,
            "prefilter.vert.spv",
            "prefilter.frag.spv",
            prefilterLayout_,
            [&](PipelineBuilder& b) {
                b.setColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
                    .disableBlending()
                    .disableDepthTest()
                    .disableDepthWrite()
                    .setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });

        shadowPipeline_ = loadPipelineByName(
            ctx_, "shadow.vert.spv", "shadow.frag.spv", shadowLayout_, [&](PipelineBuilder& b) {
                b.setVertexInput({attrs.begin(), attrs.end()}, bind)
                    .setDepthFormat(swapchain_.depthFormat())
                    .disableBlending()
                    .enableDepthTest()
                    .enableDepthWrite()
                    .enableDepthBias()
                    .setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            });
    }

    void VulkanRenderer::ensureSkyboxSet(FrameData& frame,
                                         const GpuTexture& cube,
                                         Handle<Texture> handle)
    {
        if (skyboxSet_ != VK_NULL_HANDLE && skyboxCube_ == handle)
            return; // already built for this cube

        if (skyboxSet_ == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.environmentSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &skyboxSet_));
        }

        VkDescriptorImageInfo img{};
        img.imageView = cube.view;
        img.sampler = cube.sampler;
        img.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = skyboxSet_;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &img;
        vkUpdateDescriptorSets(ctx_.device(), 1, &write, 0, nullptr);

        skyboxCube_ = handle;
    }

    void VulkanRenderer::bakeIrradiance(const GpuTexture& envCube)
    {
        {
            VkDescriptorImageInfo img{};
            img.imageView = envCube.view; // the skybox
            img.sampler = envCube.sampler;
            img.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkWriteDescriptorSet w{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            w.dstSet = environmentSet_;
            w.dstBinding = 0;
            w.descriptorCount = 1;
            w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            w.pImageInfo = &img;
            vkUpdateDescriptorSets(ctx_.device(), 1, &w, 0, nullptr);
        }

        immediateSubmit(ctx_, [&](VkCommandBuffer cmd) {
            imageBarrier(cmd,
                         irradianceTarget_.image,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                         0,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         6);

            for (uint32_t face = 0; face < 6; ++face) {
                VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                color.imageView = irradianceTarget_.renderViews[face]; // 2D single-layer view
                color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                color.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                ri.renderArea = {{0, 0}, {kIrradianceSize, kIrradianceSize}};
                ri.layerCount = 1;
                ri.colorAttachmentCount = 1;
                ri.pColorAttachments = &color;

                vkCmdBeginRendering(cmd, &ri);
                VkViewport vp{0, 0, float(kIrradianceSize), float(kIrradianceSize), 0.f, 1.f};
                vkCmdSetViewport(cmd, 0, 1, &vp);
                VkRect2D sc{{0, 0}, {kIrradianceSize, kIrradianceSize}};
                vkCmdSetScissor(cmd, 0, 1, &sc);

                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, irradiancePipeline_);
                vkCmdBindDescriptorSets(cmd,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        irradianceLayout_,
                                        0,
                                        1,
                                        &environmentSet_,
                                        0,
                                        nullptr);

                int faceIndex = int(face);
                vkCmdPushConstants(cmd,
                                   irradianceLayout_,
                                   VK_SHADER_STAGE_FRAGMENT_BIT,
                                   0,
                                   sizeof(int),
                                   &faceIndex);

                vkCmdDraw(cmd, 3, 1, 0, 0);
                vkCmdEndRendering(cmd);
            }

            imageBarrier(cmd,
                         irradianceTarget_.image,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                         VK_ACCESS_2_SHADER_READ_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         6);
        });
    }

    void VulkanRenderer::bakeBrdfLut()
    {
        immediateSubmit(ctx_, [&](VkCommandBuffer cmd) {
            imageBarrier(cmd,
                         brdfLut_.image,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                         0,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         1);

            VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
            color.imageView = brdfLut_.view;
            color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            color.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
            ri.renderArea = {{0, 0}, {kBRDFLUT, kBRDFLUT}};
            ri.layerCount = 1;
            ri.colorAttachmentCount = 1;
            ri.pColorAttachments = &color;

            vkCmdBeginRendering(cmd, &ri);
            VkViewport vp{0, 0, float(kBRDFLUT), float(kBRDFLUT), 0.f, 1.f};
            vkCmdSetViewport(cmd, 0, 1, &vp);
            VkRect2D sc{{0, 0}, {kBRDFLUT, kBRDFLUT}};
            vkCmdSetScissor(cmd, 0, 1, &sc);

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, brdfLutPipeline_);

            vkCmdDraw(cmd, 3, 1, 0, 0);
            vkCmdEndRendering(cmd);

            imageBarrier(cmd,
                         brdfLut_.image,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                         VK_ACCESS_2_SHADER_READ_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         1);
        });
    }

    void VulkanRenderer::writeEnvironmentSet(const GpuTexture& skybox)
    {
        if (environmentSet_ == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.environmentSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &environmentSet_));
        }

        VkDescriptorImageInfo imgs[4]{};
        imgs[0].imageView = skybox.view; // binding 0: skybox
        imgs[0].sampler = skybox.sampler;
        imgs[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgs[1].imageView = irradianceTarget_.view; // binding 1: irradiance
        imgs[1].sampler = irradianceTarget_.sampler;
        imgs[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgs[2].imageView = brdfLut_.view; // binding 2: brdf lut
        imgs[2].sampler = brdfLut_.sampler;
        imgs[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgs[3].imageView = prefilterTarget_.view; // binding 3: prefilter
        imgs[3].sampler = prefilterTarget_.sampler;
        imgs[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writes[4]{};
        for (int i = 0; i < 4; ++i) {
            writes[i] = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            writes[i].dstSet = environmentSet_;
            writes[i].dstBinding = uint32_t(i);
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[i].pImageInfo = &imgs[i];
        }
        vkUpdateDescriptorSets(ctx_.device(), 4, writes, 0, nullptr);
    }

    void VulkanRenderer::bakePrefilter(const GpuTexture& envCube)
    {
        const uint32_t kPrefilterMips = 5;
        if (prefilterSet_ == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.prefilterSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &prefilterSet_));
        }
        {
            VkDescriptorImageInfo img{};
            img.imageView = envCube.view; // the skybox
            img.sampler = envCube.sampler;
            img.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkWriteDescriptorSet w{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            w.dstSet = prefilterSet_;
            w.dstBinding = 0;
            w.descriptorCount = 1;
            w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            w.pImageInfo = &img;
            vkUpdateDescriptorSets(ctx_.device(), 1, &w, 0, nullptr);
        }

        immediateSubmit(ctx_, [&](VkCommandBuffer cmd) {
            imageBarrier(cmd,
                         prefilterTarget_.image,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                         0,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         6,
                         kPrefilterMips);

            for (uint32_t mip = 0; mip < kPrefilterMips; ++mip) {
                uint32_t mipSize = kPrefilterSize >> mip; // 128, 64, 32, 16, 8
                float roughness = float(mip) / float(kPrefilterMips - 1);

                for (uint32_t face = 0; face < 6; ++face) {
                    VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                    color.imageView = prefilterTarget_.renderView(mip, face);
                    color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    color.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                    VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                    ri.renderArea = {{0, 0}, {mipSize, mipSize}};
                    ri.layerCount = 1;
                    ri.colorAttachmentCount = 1;
                    ri.pColorAttachments = &color;

                    vkCmdBeginRendering(cmd, &ri);
                    // viewport + scissor sized to THIS mip (not the base size!)
                    VkViewport vp{0, 0, float(mipSize), float(mipSize), 0.f, 1.f};
                    vkCmdSetViewport(cmd, 0, 1, &vp);
                    VkRect2D sc{{0, 0}, {mipSize, mipSize}};
                    vkCmdSetScissor(cmd, 0, 1, &sc);

                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, prefilterPipeline_);
                    vkCmdBindDescriptorSets(cmd,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            prefilterLayout_,
                                            0,
                                            1,
                                            &prefilterSet_,
                                            0,
                                            nullptr);

                    struct {
                        int face;
                        float rough;
                    } push{int(face), roughness};
                    vkCmdPushConstants(cmd,
                                       prefilterLayout_,
                                       VK_SHADER_STAGE_FRAGMENT_BIT,
                                       0,
                                       sizeof(push),
                                       &push);
                    vkCmdDraw(cmd, 3, 1, 0, 0);
                    vkCmdEndRendering(cmd);
                }
            }

            imageBarrier(cmd,
                         prefilterTarget_.image,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                         VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                         VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                         VK_ACCESS_2_SHADER_READ_BIT,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         6,
                         kPrefilterMips);
        });
    }

    void VulkanRenderer::shadowMapping(VkCommandBuffer cmd,
                                       const std::vector<uint32_t>& order,
                                       const FrameRenderData& renderData)
    {
        auto& target = frames_[currentFrame_].shadowTarget;
        const auto& items = renderData.items;

        {
            VkDescriptorImageInfo img{};
            img.imageView = target.view; // the skybox
            img.sampler = target.sampler;
            img.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkWriteDescriptorSet w{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            w.dstSet = frames_[currentFrame_].lightSet;
            w.dstBinding = 1;
            w.descriptorCount = 1;
            w.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            w.pImageInfo = &img;
            vkUpdateDescriptorSets(ctx_.device(), 1, &w, 0, nullptr);
        }

        imageBarrier(cmd,
                     target.image,
                     VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                     VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                     0,
                     VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                         VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                     VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                     VK_IMAGE_ASPECT_DEPTH_BIT, // no stencil
                     1,
                     1);

        VkRenderingAttachmentInfo depth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depth.imageView = target.view;
        depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth.clearValue.depthStencil.depth = 1.0f;

        VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
        ri.renderArea = {{0, 0}, {kShadowMapSize, kShadowMapSize}};
        ri.layerCount = 1;
        ri.pDepthAttachment = &depth;

        profiler_.beginRegion(cmd, "Shadow Pass");
        vkCmdBeginRendering(cmd, &ri);
        VkViewport vp{0, 0, float(kShadowMapSize), float(kShadowMapSize), 0.f, 1.f};
        vkCmdSetViewport(cmd, 0, 1, &vp);
        VkRect2D sc{{0, 0}, {kShadowMapSize, kShadowMapSize}};
        vkCmdSetScissor(cmd, 0, 1, &sc);

        vkCmdSetDepthBias(cmd, 1.25f, 0.f, 2.f);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline_);

        for (auto& idx : order) {
            const RenderItem& item = items[idx];

            const GpuMesh* mesh = meshCache_->resource(item.mesh);
            if (!mesh)
                continue;

            struct {
                math::Mat4 proj;
                math::Mat4 view;
                math::Mat4 model;
            } push{renderData.sun.proj, renderData.sun.view, item.model};
            vkCmdPushConstants(
                cmd, shadowLayout_, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(push), &push);
            VkBuffer vb = mesh->vertexBuffer.handle;
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(cmd, 0, 1, &vb, &offset);
            vkCmdBindIndexBuffer(cmd, mesh->indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cmd, mesh->indexCount, 1, 0, 0, 0);
            stats_.shadowPass.drawCalls++;
        }

        vkCmdEndRendering(cmd);
        profiler_.endRegion(cmd, "Shadow Pass");

        imageBarrier(cmd,
                     target.image,
                     VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                     VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                     VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                     VK_ACCESS_2_SHADER_READ_BIT,
                     VK_IMAGE_ASPECT_DEPTH_BIT, // No stencil
                     1,
                     1);
    }

    ViewportTarget VulkanRenderer::createViewportTarget(VkExtent2D extent)
    {
        ViewportTarget target;
        target.view.extent = extent;
        target.view.colorFormat = swapchain_.format();
        target.view.depthFormat = swapchain_.depthFormat();
        target.view.clearColor.color = {{0.02f, 0.02f, 0.03f, 1.0f}};

        // --- color ---
        VkImageCreateInfo colorInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        colorInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        colorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        colorInfo.imageType = VK_IMAGE_TYPE_2D;
        colorInfo.format = target.view.colorFormat;
        colorInfo.extent = {extent.width, extent.height, 1};
        colorInfo.mipLevels = 1;
        colorInfo.arrayLayers = 1;
        colorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        colorInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        VmaAllocationCreateInfo colorAlloc{};
        colorAlloc.usage = VMA_MEMORY_USAGE_AUTO;

        VK_CHECK(vmaCreateImage(ctx_.allocator(),
                                &colorInfo,
                                &colorAlloc,
                                &target.view.image,
                                &target.colorAlloc,
                                nullptr));

        VkImageViewCreateInfo colorViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        colorViewInfo.image = target.view.image;
        colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorViewInfo.format = target.view.colorFormat;
        colorViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        VK_CHECK(vkCreateImageView(ctx_.device(), &colorViewInfo, nullptr, &target.view.colorView));

        // --- depth ---
        VkImageCreateInfo depthInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        depthInfo.imageType = VK_IMAGE_TYPE_2D;
        depthInfo.format = target.view.depthFormat;
        depthInfo.extent = {extent.width, extent.height, 1};
        depthInfo.mipLevels = 1;
        depthInfo.arrayLayers = 1;
        depthInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VmaAllocationCreateInfo depthAlloc{};
        depthAlloc.usage = VMA_MEMORY_USAGE_AUTO;

        VK_CHECK(vmaCreateImage(ctx_.allocator(),
                                &depthInfo,
                                &depthAlloc,
                                &target.view.depthImage,
                                &target.depthAlloc,
                                nullptr));

        VkImageViewCreateInfo depthViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        depthViewInfo.image = target.view.depthImage;
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = target.view.depthFormat;
        depthViewInfo.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        VK_CHECK(vkCreateImageView(ctx_.device(), &depthViewInfo, nullptr, &target.view.depthView));

        return target;
    }

    void VulkanRenderer::recreateViewportTarget(Viewport& viewport, uint32_t frameIndex)
    {
        ViewportTarget& target = viewport.targets[frameIndex];

        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)target.imguiTextureId);
        target.destroy(ctx_);

        target = createViewportTarget(viewport.pendingExtent);
        target.imguiTextureId = (ImTextureID)ImGui_ImplVulkan_AddTexture(
            linearSampler_, target.view.colorView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        viewport.everRendered[frameIndex] = false; // fresh image, back to Undefined
    }

    void VulkanRenderer::createCameraUBO(VkBuffer& buffer,
                                         VmaAllocation& alloc,
                                         void*& mapped,
                                         VkDescriptorSet& set)
    {
        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.size = sizeof(CameraData);
        bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo aci{};
        aci.usage = VMA_MEMORY_USAGE_AUTO;
        aci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo{};
        VK_CHECK(vmaCreateBuffer(ctx_.allocator(), &bufInfo, &aci, &buffer, &alloc, &allocInfo));
        mapped = allocInfo.pMappedData;

        VkDescriptorSetLayout camLayout = ctx_.cameraSetLayout();
        VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        dsai.descriptorPool = ctx_.descriptorPool();
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &camLayout;
        VK_CHECK(vkAllocateDescriptorSets(ctx_.device(), &dsai, &set));

        VkDescriptorBufferInfo dbi{buffer, 0, sizeof(CameraData)};
        VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = set;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &dbi;
        vkUpdateDescriptorSets(ctx_.device(), 1, &write, 0, nullptr);
    }

    VulkanRenderer::Viewport VulkanRenderer::createViewport(const std::string& id,
                                                            uint32_t cameraViewId)
    {
        Viewport vp;
        vp.id = id;
        vp.cameraViewId = cameraViewId;

        for (int i = 0; i < kFramesInFlight; i++) {
            vp.targets[i] = createViewportTarget(
                swapchain_.extent());
            vp.targets[i].imguiTextureId =
                (ImTextureID)ImGui_ImplVulkan_AddTexture(linearSampler_,
                                                         vp.targets[i].view.colorView,
                                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            createCameraUBO(
                vp.cameraBuffer[i], vp.cameraAlloc[i], vp.cameraMapped[i], vp.cameraSet[i]);
        }
        return vp;
    }

    void VulkanRenderer::destroyViewport(Viewport& vp)
    {
        waitIdle(); // ehh idk

        for (int i = 0; i < kFramesInFlight; i++) {
            ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)vp.targets[i].imguiTextureId);
            vp.targets[i].destroy(ctx_);

            vkDestroyBuffer(ctx_.device(),
                            vp.cameraBuffer[i],
                            nullptr);
            vmaFreeMemory(ctx_.allocator(), vp.cameraAlloc[i]);
        }
    }

    ViewportHandle VulkanRenderer::addViewport(const std::string& id, uint32_t cameraViewId)
    {
        uint32_t index;
        if (!freeViewportSlots_.empty()) {
            index = freeViewportSlots_.back();
            freeViewportSlots_.pop_back();
        } else {
            index = static_cast<uint32_t>(viewportSlots_.size());
            viewportSlots_.emplace_back();
        }

        ViewportSlot& slot = viewportSlots_[index];
        slot.viewport = createViewport(
            id, cameraViewId);
        slot.alive = true;

        return ViewportHandle{index, slot.generation};
    }

    void VulkanRenderer::removeViewport(ViewportHandle handle)
    {
        if (!isValidHandle(handle))
            return;

        ViewportSlot& slot = viewportSlots_[handle.index];
        destroyViewport(slot.viewport);

        slot.alive = false;
        slot.generation++;
        freeViewportSlots_.push_back(handle.index);
    }

    bool VulkanRenderer::isValidHandle(ViewportHandle handle) const
    {
        return handle.valid() && handle.index < viewportSlots_.size() &&
               viewportSlots_[handle.index].alive &&
               viewportSlots_[handle.index].generation == handle.generation;
    }

    VulkanRenderer::Viewport* VulkanRenderer::getViewport(ViewportHandle handle)
    {
        return isValidHandle(handle) ? &viewportSlots_[handle.index].viewport : nullptr;
    }

    uint64_t VulkanRenderer::getViewportTextureId(ViewportHandle handle) const
    {
        auto* vp = const_cast<VulkanRenderer*>(this)->getViewport(handle);
        return vp ? (uint64_t)vp->targets[currentFrame_].imguiTextureId : 0;
    }
    void VulkanRenderer::setViewportHovered(ViewportHandle handle, bool hovered)
    {
        if (auto* vp = getViewport(handle))
            vp->hovered = hovered;
    }
    void VulkanRenderer::requestViewportResize(ViewportHandle handle, uint32_t w, uint32_t h)
    {
        if (auto* vp = getViewport(handle)) {
            vp->pendingExtent = {w, h};
            vp->needsResize = true;
        }
    }

    void applyCustomStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // ---- Palette: cool dark neutrals, single accent, no default ImGui blue/gray ----
        ImVec4 bg = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        ImVec4 bgLight = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
        ImVec4 bgLighter = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
        ImVec4 border = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
        ImVec4 text = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
        ImVec4 textDim = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);
        ImVec4 accent = ImVec4(0.35f, 0.65f, 0.95f, 1.00f);
        ImVec4 accentHover = ImVec4(0.45f, 0.72f, 1.00f, 1.00f);
        ImVec4 accentActive = ImVec4(0.28f, 0.55f, 0.85f, 1.00f);

        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = textDim;
        colors[ImGuiCol_WindowBg] = bgLight;
        colors[ImGuiCol_ChildBg] = bgLight;
        colors[ImGuiCol_PopupBg] = bgLight;
        colors[ImGuiCol_Border] = border;
        colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_FrameBg] = bg;
        colors[ImGuiCol_FrameBgHovered] = bgLighter;
        colors[ImGuiCol_FrameBgActive] = bgLighter;
        colors[ImGuiCol_TitleBg] = bg;
        colors[ImGuiCol_TitleBgActive] = bg;
        colors[ImGuiCol_TitleBgCollapsed] = bg;
        colors[ImGuiCol_MenuBarBg] = bgLight;
        colors[ImGuiCol_ScrollbarBg] = bg;
        colors[ImGuiCol_ScrollbarGrab] = bgLighter;
        colors[ImGuiCol_ScrollbarGrabHovered] = border;
        colors[ImGuiCol_ScrollbarGrabActive] = accent;
        colors[ImGuiCol_CheckMark] = accent;
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = accentActive;
        colors[ImGuiCol_Button] = bgLighter;
        colors[ImGuiCol_ButtonHovered] = accentHover;
        colors[ImGuiCol_ButtonActive] = accentActive;
        colors[ImGuiCol_Header] = bgLighter;
        colors[ImGuiCol_HeaderHovered] = accentHover;
        colors[ImGuiCol_HeaderActive] = accentActive;
        colors[ImGuiCol_Separator] = border;
        colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ResizeGripHovered] = accent;
        colors[ImGuiCol_ResizeGripActive] = accentActive;
        colors[ImGuiCol_DockingPreview] =
            ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_DockingEmptyBg] = bg;
        colors[ImGuiCol_Tab] = bgLight;
        colors[ImGuiCol_TabHovered] = accentHover;
        colors[ImGuiCol_TabSelected] = accent;
        colors[ImGuiCol_TabDimmed] = bgLight;
        colors[ImGuiCol_TabSelectedOverline] = accent;
        colors[ImGuiCol_TabDimmedSelected] = bgLight;
        colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
        colors[ImGuiCol_TabActive] = bgLight;
        //colors[ImGuiCol_TabUnfocused] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
        //colors[ImGuiCol_TabUnfocusedActive] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
        colors[ImGuiCol_PlotLines] = accent;
        colors[ImGuiCol_PlotHistogram] = accent;
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_NavCursor] = accent;

        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 1.0f;
        style.TabBarBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        style.TabBarOverlineSize = 2.0f;
        style.TabCloseButtonMinWidthSelected = -1.0f;
        style.DockingNodeHasCloseButton = false;

        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.PopupBorderSize = 0.0f;
        style.ChildBorderSize = 1.0f;

        style.WindowPadding = ImVec2(13, 13);
        style.FramePadding = ImVec2(10, 6);
        style.ItemSpacing = ImVec2(10, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 14.0f;

        style.GrabMinSize =
            12.0f;
        style.ScrollbarSize = 12.0f;
    }

    bool VulkanRenderer::initializeUI()
    {
        ImGui::CreateContext();
        //TODO: not ideal, i dont want glfw here at all
        ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window_.nativeHandle()), false);

        const auto format = swapchain_.format();

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = ctx_.instance();
        initInfo.PhysicalDevice = ctx_.physicalDevice();
        initInfo.Device = ctx_.device();
        initInfo.QueueFamily = ctx_.graphicsFamily();
        initInfo.Queue = ctx_.graphicsQueue();
        initInfo.DescriptorPool = VK_NULL_HANDLE;
        initInfo.DescriptorPoolSize = 8;
        initInfo.MinImageCount = kFramesInFlight;
        initInfo.ImageCount = kFramesInFlight;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;
        ImGui_ImplVulkan_Init(&initInfo);

        //make ImGui not look terrible
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig config;
        config.OversampleH = 4;
        config.OversampleV = 4;

        applyCustomStyle();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        return true;
    }

    void VulkanRenderer::shutdownUI()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void VulkanRenderer::beginUIFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanRenderer::endUIFrame()
    {
        ImGui::Render();
    }

    void VulkanRenderer::renderUI(VkCommandBuffer cmd, VkImageView imageView)
    {
        VkRenderingAttachmentInfo uiColorAttachment{};
        uiColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        uiColorAttachment.imageView = imageView; // whatever your accessor is
        uiColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        uiColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        uiColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        uiColorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

        VkRenderingInfo uiRenderingInfo{};
        uiRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        uiRenderingInfo.renderArea = {{0, 0}, swapchain_.extent()};
        uiRenderingInfo.layerCount = 1;
        uiRenderingInfo.colorAttachmentCount = 1;
        uiRenderingInfo.pColorAttachments = &uiColorAttachment;

        profiler_.beginRegion(cmd, "UI Rendering");
        vkCmdBeginRendering(cmd, &uiRenderingInfo);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
        vkCmdEndRendering(cmd);
        profiler_.endRegion(cmd, "UI Rendering");
    }
} // namespace chai::gfx