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
#include <Input/IInput.h>
#include <Log.h>
#include <Rendering/CameraData.h>
#include <Runtime/SystemPaths.h>
#include <Window/Window.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <numeric>
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
                                   std::shared_ptr<ViewportRegistry> viewportReg,
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
          modelReg_(texReg), viewportReg_(*viewportReg), pipelineReg_(context), renderGraph_(ctx_)
    {
        init();
        CHAI_LOG_INFO("VulkanRenderer initialized");
    }

    VulkanRenderer::~VulkanRenderer()
    {
        waitIdle();

        profiler_.shutdown(ctx_.device());

        for (int i = 0; i < kFramesInFlight; i++) {
            vmaDestroyBuffer(ctx_.allocator(), frames_[i].lightBuffer, frames_[i].lightAlloc);
            vkDestroyFence(ctx_.device(), frames_[i].inFlight, nullptr);
            vkDestroySemaphore(ctx_.device(), frames_[i].imageAvailable, nullptr);
            frames_[i].shadowTarget.destroy(ctx_);
        }

        pipelineReg_.destroyAll();

        vkDestroyPipelineLayout(ctx_.device(), pipelineLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), irradianceLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), brdfLutLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), prefilterLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), shadowLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), thresholdLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), combineLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), bloomLayout_, nullptr);
        vkDestroyPipelineLayout(ctx_.device(), pbrLayout_, nullptr);

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

            const uint32_t kBloomMips = 6;
            uint32_t bloomSetCount = 2 * (kBloomMips - 1);
            frames_[i].bloomSampleSets.resize(bloomSetCount);
            std::vector<VkDescriptorSetLayout> layouts(bloomSetCount, ctx_.bloomSampleSetLayout());
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = bloomSetCount;
            dsai.pSetLayouts = layouts.data();
            VK_CHECK(
                vkAllocateDescriptorSets(ctx_.device(), &dsai, frames_[i].bloomSampleSets.data()));

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

        // TODO: I dont like that theres no way out. Need to rethink how to default bind the
        // environment Or if default binding (to a black skybox cube) makes sense.
        while (!texCache_->isReady(cube)) {
            texCache_->tick();
        }
        auto skyResource = texCache_->resource(cube);
        bakeIrradiance(*skyResource);
        bakePrefilter(*skyResource);
        writeEnvironmentSet(*skyResource);
        iblBaked_ = true;

        viewportReg_.init(swapchain_);
    }

    void VulkanRenderer::startFrame()
    {
        FrameData& frame = frames_[currentFrame_];

        // Wait until this frame slots previous work is done.
        VK_CHECK(vkWaitForFences(ctx_.device(), 1, &frame.inFlight, VK_TRUE, UINT64_MAX));

        viewportReg_.tick(currentFrame_);
        viewportReg_.applyPendingViewportResizes();

        // sync input to imgui
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

        auto focusedViewport = viewportReg_.getFocusedViewport();
        if (focusedViewport.has_value())
            input->setHoveredCamera(focusedViewport.value().cameraViewId);
        else {
            input->setHoveredCamera(-1);
        }

        beginUIFrame();
    }

    void VulkanRenderer::endFrame()
    {
        // nothing to do for now
    }

    void VulkanRenderer::renderFrame(const FrameRenderData& renderData)
    {
        ZoneScoped

            if (needsResize_) recreateSwapchain();

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

        renderGraph_.clear();
        CRGTextureHandle shadowHandle =
            renderGraph_.importTexture("ShadowMap",
                                       frames_[currentFrame_].shadowTarget,
                                       ImageState::Undefined,
                                       TextureType::Depth);
        shadowMapping(cmd, order, renderData, shadowHandle);

        renderGraph_.compile();
        renderGraph_.execute(cmd);

        viewportReg_.forEachViewport([&](Viewport& viewport) {
            renderGraph_.clear();

            ViewportTarget& target = viewport.targets[currentFrame_];
            auto viewIt = std::find_if(renderData.views.begin(),
                                       renderData.views.end(),
                                       [camId = viewport.cameraViewId](const RenderView& data) {
                                           return data.cameraId == camId;
                                       });
            if (viewIt == renderData.views.end()) {
                CHAI_LOG_WARN("Invalid Camera Id for Viewport {}", viewport.id);
                return;
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

            constexpr VkFormat kBloomFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            constexpr VkFormat kSceneDepthFormat = VK_FORMAT_D32_SFLOAT;

            // Need 2 textures: one for scene color and one for MSAA. Both for color and depth
            CRGTextureHandle sceneHDR = renderGraph_.createTexture(
                "SceneColorHDR",
                {target.view.extent.width, target.view.extent.height, kBloomFormat, 1});
            CRGTextureHandle sceneHDRMSAA =
                renderGraph_.createTexture("SceneColorHDRMulti",
                                           {target.view.extent.width,
                                            target.view.extent.height,
                                            kBloomFormat,
                                            1,
                                            TextureType::Color2D,
                                            ctx_.getSampleCount(VK_SAMPLE_COUNT_4_BIT)});
            CRGTextureHandle sceneDepth = renderGraph_.createTexture("SceneDepth",
                                                                     {target.view.extent.width,
                                                                      target.view.extent.height,
                                                                      kSceneDepthFormat,
                                                                      1,
                                                                      TextureType::Depth});
            CRGTextureHandle sceneDepthMulti =
                renderGraph_.createTexture("SceneDepthMulti",
                                           {target.view.extent.width,
                                            target.view.extent.height,
                                            kSceneDepthFormat,
                                            1,
                                            TextureType::Depth,
                                            ctx_.getSampleCount(VK_SAMPLE_COUNT_4_BIT)});

            struct MainPassData {
                CRGTextureHandle color, colorMulti, depth, depthMulti, shadow;
            };
            renderGraph_.addPass<MainPassData>(
                "MainPass_" + viewport.id,
                [&](CRGBuilder& builder, MainPassData& data) {
                    data.color = builder.write(sceneHDR);
                    data.depth = builder.write(sceneDepth);
                    data.shadow = builder.read(shadowHandle);
                    data.colorMulti = builder.write(sceneHDRMSAA);
                    data.depthMulti = builder.write(sceneDepthMulti);
                },
                [&, order](
                    VkCommandBuffer cmd, const MainPassData& data, const CRGResources& resources) {
                    VkExtent2D extent = resources.extent(data.color, 0);

                    VkRenderingAttachmentInfo vpColor{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                    vpColor.imageView = resources.attachmentView(data.colorMulti, 0);
                    vpColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    vpColor.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                    vpColor.resolveImageView = resources.attachmentView(data.color, 0);
                    vpColor.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    vpColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    vpColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    vpColor.clearValue = target.view.clearColor;

                    VkRenderingAttachmentInfo vpDepth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                    vpDepth.imageView = resources.attachmentView(data.depthMulti, 0);
                    vpDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    vpDepth.resolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
                    vpDepth.resolveImageView = resources.attachmentView(data.depth, 0);
                    vpDepth.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    vpDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    vpDepth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    vpDepth.clearValue.depthStencil = {1.0f, 0};

                    VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                    ri.renderArea = {{0, 0}, extent};
                    ri.layerCount = 1;
                    ri.colorAttachmentCount = 1;
                    ri.pColorAttachments = &vpColor;
                    ri.pDepthAttachment = &vpDepth;

                    RenderTargetView sceneView{};
                    sceneView.extent = extent;
                    sceneView.colorFormat = kBloomFormat;

                    profiler_.beginRegion(cmd, "Main Pass: " + viewport.id);
                    vkCmdBeginRendering(cmd, &ri);
                    renderScene(cmd,
                                sceneView,
                                renderData,
                                order,
                                viewport.cameraSet[currentFrame_],
                                viewport.shadingMode,
                                viewport.wireframe);
                    vkCmdEndRendering(cmd);
                    profiler_.endRegion(cmd, "Main Pass: " + viewport.id);
                });

            bool everBlitted = viewport.everRendered[currentFrame_];
            viewport.everRendered[currentFrame_] = true;

            CRGTextureHandle bloomChain = renderGraph_.createTexture(
                "BloomChain",
                {target.view.extent.width, target.view.extent.height, kBloomFormat, 6});

            CRGTextureHandle combineTarget = renderGraph_.createTexture(
                "CombineTarget",
                {target.view.extent.width, target.view.extent.height, kBloomFormat, 1});

            profiler_.beginRegion(cmd, "Post Process Pass: " + viewport.id);
            bloomPass(renderGraph_,
                      target.view,
                      sceneHDR,
                      bloomChain); // sceneHDR directly, no import needed
            combinePass(renderGraph_, target.view, sceneHDR, bloomChain, combineTarget);
            profiler_.endRegion(cmd, "Post Process Pass: " + viewport.id);

            renderGraph_.compile();
            renderGraph_.execute(cmd);

            blitCombineToViewport(cmd,
                                  target.view,
                                  renderGraph_.resolvedImage(combineTarget),
                                  renderGraph_.resolvedExtent(combineTarget),
                                  everBlitted);
            renderGraph_.markExternalState(combineTarget, ImageState::TransferSrc);
        });

        // Render UI
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
                                     VkDescriptorSet cameraSet,
                                     ViewportShadingMode shadingMode,
                                     bool wireframe)
    {
        VkViewport viewport{0, 0, float(view.extent.width), float(view.extent.height), 0.f, 1.f};
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        VkRect2D scissor{{0, 0}, view.extent};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        FrameData& frame = frames_[currentFrame_];

        // Global sets, bound once
        vkCmdBindDescriptorSets(
            cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &cameraSet, 0, nullptr);
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

            VkPipeline pipeline; 
            if (wireframe) {
                pipeline = pipelineReg_.get(wireframeHandle_);
            } else if (mat->alphaMode == AlphaMode::Blend) {
                pipeline = pipelineReg_.get(pbrBlendHandle_);
            } else {
                pipeline = pipelineReg_.get(pbrOpaqueHandle_);
            }

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

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
            consts.shadingMode = static_cast<int>(shadingMode);
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

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(skyboxHandle_));

        vkCmdBindDescriptorSets(
            cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 3, 1, &skyboxSet_, 0, nullptr);

        vkCmdDraw(cmd, 3, 1, 0, 0);
    }

    void VulkanRenderer::bloomPass(ChaiRenderGraph& renderGraph,
                                   RenderTargetView& target,
                                   CRGTextureHandle& sceneHandle,
                                   CRGTextureHandle& bloomChain)
    {
        // Soft threshold
        struct ThresholdData {
            CRGTextureHandle scene, output;
        };
        renderGraph.addPass<ThresholdData>(
            "BloomThreshold",
            [&](CRGBuilder& builder, ThresholdData& data) {
                data.scene = builder.read(sceneHandle);
                data.output = builder.write(bloomChain, /*mip=*/0);
            },
            [&](VkCommandBuffer cmd, const ThresholdData& data, const CRGResources& resources) {
                VkRenderingAttachmentInfo att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                att.imageView = resources.attachmentView(data.output, 0);
                att.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                VkExtent2D extent = resources.extent(data.output, 0);
                VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                ri.renderArea = {{0, 0}, extent};
                ri.layerCount = 1;
                ri.colorAttachmentCount = 1;
                ri.pColorAttachments = &att;

                vkCmdBeginRendering(cmd, &ri);
                VkViewport vp{0, 0, float(extent.width), float(extent.height), 0.f, 1.f};
                vkCmdSetViewport(cmd, 0, 1, &vp);
                VkRect2D sc{{0, 0}, extent};
                vkCmdSetScissor(cmd, 0, 1, &sc);

                setupThreshold(resources.view(data.scene));
                vkCmdBindPipeline(
                    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(softThresholdHandle_));
                vkCmdBindDescriptorSets(cmd,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        thresholdLayout_,
                                        0,
                                        1,
                                        &frames_[currentFrame_].bloomThresholdSet,
                                        0,
                                        nullptr);
                vkCmdDraw(cmd, 3, 1, 0, 0);
                vkCmdEndRendering(cmd);
            });

        uint32_t bloomSetIndex_ = 0;

        // Downsample chain
        CRGTextureHandle downsampleOutput =
            bloomChain; // tracks the "current" handle through the chain
        for (uint32_t mip = 1; mip < 6; ++mip) {
            uint32_t srcMip = mip - 1;

            const uint32_t bloomSetIndex = bloomSetIndex_++;

            struct DownsampleData {
                CRGTextureHandle src, dst;
            };
            renderGraph.addPass<DownsampleData>(
                "BloomDownsample" + std::to_string(mip),
                [&, srcMip, mip](CRGBuilder& builder, DownsampleData& data) {
                    data.src = builder.read(bloomChain, srcMip);
                    data.dst = builder.write(bloomChain, mip);
                },
                [&, srcMip, mip, bloomSetIndex](VkCommandBuffer cmd,
                                                const DownsampleData& data,
                                                const CRGResources& resources) {
                    VkExtent2D srcExtent = resources.extent(data.src, srcMip);
                    VkExtent2D dstExtent = resources.extent(data.dst, mip);

                    VkRenderingAttachmentInfo att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                    att.imageView = resources.attachmentView(data.dst, mip);
                    att.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    att.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                    VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                    ri.renderArea = {{0, 0}, dstExtent};
                    ri.layerCount = 1;
                    ri.colorAttachmentCount = 1;
                    ri.pColorAttachments = &att;

                    vkCmdBeginRendering(cmd, &ri);
                    VkViewport vp{0, 0, float(dstExtent.width), float(dstExtent.height), 0.f, 1.f};
                    vkCmdSetViewport(cmd, 0, 1, &vp);
                    VkRect2D sc{{0, 0}, dstExtent};
                    vkCmdSetScissor(cmd, 0, 1, &sc);

                    VkDescriptorSet set = frames_[currentFrame_].bloomSampleSets[bloomSetIndex];
                    VkDescriptorImageInfo img{ctx_.linearSampler(),
                                              resources.view(data.src, srcMip),
                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
                    VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                    write.dstSet = set;
                    write.dstBinding = 0;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo = &img;
                    vkUpdateDescriptorSets(ctx_.device(), 1, &write, 0, nullptr);

                    vkCmdBindPipeline(
                        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(downsampleHandle_));
                    vkCmdBindDescriptorSets(
                        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, bloomLayout_, 0, 1, &set, 0, nullptr);

                    struct {
                        float x, y;
                    } pc{1.0f / srcExtent.width, 1.0f / srcExtent.height};
                    vkCmdPushConstants(
                        cmd, bloomLayout_, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pc), &pc);
                    vkCmdDraw(cmd, 3, 1, 0, 0);
                    vkCmdEndRendering(cmd);
                });
        }

        // Upsample chain
        for (int mip = 4; mip >= 0; --mip) {
            uint32_t srcMip = uint32_t(mip) + 1;

            const uint32_t bloomSetIndex = bloomSetIndex_++;

            struct UpsampleData {
                CRGTextureHandle src, dst;
            };
            renderGraph.addPass<UpsampleData>(
                "BloomUpsample" + std::to_string(mip),
                [&, srcMip, mip](CRGBuilder& builder, UpsampleData& data) {
                    data.src = builder.read(bloomChain, srcMip);
                    data.dst = builder.write(bloomChain, uint32_t(mip));
                },
                [&, srcMip, mip, bloomSetIndex](
                    VkCommandBuffer cmd, const UpsampleData& data, const CRGResources& resources) {
                    VkExtent2D srcExtent = resources.extent(data.src, srcMip);
                    VkExtent2D dstExtent = resources.extent(data.dst, uint32_t(mip));

                    VkRenderingAttachmentInfo att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                    att.imageView = resources.attachmentView(data.dst, uint32_t(mip));
                    att.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    att.loadOp =
                        VK_ATTACHMENT_LOAD_OP_LOAD; // keep downsample content, blend adds onto it
                    att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                    VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                    ri.renderArea = {{0, 0}, dstExtent};
                    ri.layerCount = 1;
                    ri.colorAttachmentCount = 1;
                    ri.pColorAttachments = &att;

                    vkCmdBeginRendering(cmd, &ri);
                    VkViewport vp{0, 0, float(dstExtent.width), float(dstExtent.height), 0.f, 1.f};
                    vkCmdSetViewport(cmd, 0, 1, &vp);
                    VkRect2D sc{{0, 0}, dstExtent};
                    vkCmdSetScissor(cmd, 0, 1, &sc);

                    VkDescriptorSet set = frames_[currentFrame_].bloomSampleSets[bloomSetIndex];
                    VkDescriptorImageInfo img{ctx_.linearSampler(),
                                              resources.view(data.src, srcMip),
                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
                    VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                    write.dstSet = set;
                    write.dstBinding = 0;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo = &img;
                    vkUpdateDescriptorSets(ctx_.device(), 1, &write, 0, nullptr);

                    vkCmdBindPipeline(
                        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(upsampleHandle_));
                    vkCmdBindDescriptorSets(
                        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, bloomLayout_, 0, 1, &set, 0, nullptr);

                    struct {
                        float x, y;
                    } pc{1.0f / srcExtent.width, 1.0f / srcExtent.height};
                    vkCmdPushConstants(
                        cmd, bloomLayout_, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pc), &pc);
                    vkCmdDraw(cmd, 3, 1, 0, 0);
                    vkCmdEndRendering(cmd);
                });
        }
    }

    void VulkanRenderer::setupThreshold(VkImageView sceneView)
    {
        if (frames_[currentFrame_].bloomThresholdSet == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.bloomThresholdLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(vkAllocateDescriptorSets(
                ctx_.device(), &dsai, &frames_[currentFrame_].bloomThresholdSet));
        }

        VkDescriptorImageInfo imgs{};
        imgs.imageView = sceneView; // binding 0: scene
        imgs.sampler = ctx_.linearSampler();
        imgs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writes{};
        writes = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writes.dstSet = frames_[currentFrame_].bloomThresholdSet;
        writes.dstBinding = 0;
        writes.descriptorCount = 1;
        writes.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes.pImageInfo = &imgs;

        vkUpdateDescriptorSets(ctx_.device(), 1, &writes, 0, nullptr);
    }

    void VulkanRenderer::setupCombine(VkImageView sceneView, VkImageView bloomView)
    {
        if (frames_[currentFrame_].combineSet == VK_NULL_HANDLE) {
            VkDescriptorSetLayout layout = ctx_.combineSetLayout();
            VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsai.descriptorPool = ctx_.descriptorPool();
            dsai.descriptorSetCount = 1;
            dsai.pSetLayouts = &layout;
            VK_CHECK(
                vkAllocateDescriptorSets(ctx_.device(), &dsai, &frames_[currentFrame_].combineSet));
        }

        VkDescriptorImageInfo imgs[2]{};
        imgs[0].imageView = sceneView; // binding 0: scene
        imgs[0].sampler = ctx_.linearSampler();
        imgs[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgs[1].imageView = bloomView; // binding 1: bloom
        imgs[1].sampler = ctx_.linearSampler();
        imgs[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writes[2]{};
        for (int i = 0; i < 2; ++i) {
            writes[i] = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            writes[i].dstSet = frames_[currentFrame_].combineSet;
            writes[i].dstBinding = uint32_t(i);
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[i].pImageInfo = &imgs[i];
        }

        vkUpdateDescriptorSets(ctx_.device(), 2, writes, 0, nullptr);
    }

    void VulkanRenderer::combinePass(ChaiRenderGraph& renderGraph,
                                     RenderTargetView& target,
                                     CRGTextureHandle& sceneHandle,
                                     CRGTextureHandle& bloomChain,
                                     CRGTextureHandle& combineTarget)
    {
        struct CombineData {
            CRGTextureHandle scene, bloom, output;
        };
        renderGraph.addPass<CombineData>(
            "BloomCombine",
            [&](CRGBuilder& builder, CombineData& data) {
                data.scene = builder.read(sceneHandle);
                data.bloom = builder.read(bloomChain, 0);
                data.output = builder.write(combineTarget);
            },
            [&](VkCommandBuffer cmd, const CombineData& data, const CRGResources& resources) {
                VkRenderingAttachmentInfo att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                att.imageView = resources.attachmentView(data.output, 0);
                att.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                VkExtent2D extent = resources.extent(data.output, 0);
                VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
                ri.renderArea = {{0, 0}, extent};
                ri.layerCount = 1;
                ri.colorAttachmentCount = 1;
                ri.pColorAttachments = &att;

                vkCmdBeginRendering(cmd, &ri);
                VkViewport vp{0, 0, float(extent.width), float(extent.height), 0.f, 1.f};
                vkCmdSetViewport(cmd, 0, 1, &vp);
                VkRect2D sc{{0, 0}, extent};
                vkCmdSetScissor(cmd, 0, 1, &sc);

                setupCombine(resources.view(data.scene), resources.view(data.bloom));
                vkCmdBindPipeline(
                    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(combineHandle_));
                vkCmdBindDescriptorSets(cmd,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        combineLayout_,
                                        0,
                                        1,
                                        &frames_[currentFrame_].combineSet,
                                        0,
                                        nullptr);
                vkCmdDraw(cmd, 3, 1, 0, 0);
                vkCmdEndRendering(cmd);
            });
    }

    void VulkanRenderer::blitCombineToViewport(VkCommandBuffer cmd,
                                               const RenderTargetView& view,
                                               VkImage combineImage,
                                               VkExtent2D combineExtent,
                                               bool everRendered)
    {
        transitionImage(cmd, combineImage, ImageState::ColorAttachment, ImageState::TransferSrc);
        transitionImage(cmd,
                        view.image,
                        everRendered ? ImageState::ShaderRead : ImageState::Undefined,
                        ImageState::TransferDst);

        VkImageBlit blit{};
        blit.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {int32_t(combineExtent.width), int32_t(combineExtent.height), 1};

        blit.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {int32_t(view.extent.width), int32_t(view.extent.height), 1};

        vkCmdBlitImage(cmd,
                       combineImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       view.image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &blit,
                       VK_FILTER_LINEAR);

        transitionImage(cmd, view.image, ImageState::TransferDst, ImageState::ShaderRead);
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

        {
            VkDescriptorSetLayout postSets[] = {ctx_.bloomThresholdLayout()}; // 1 sampler
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 1;
            li.pSetLayouts = postSets;
            li.pushConstantRangeCount = 0;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &thresholdLayout_));
        }

        {
            VkPushConstantRange bloomPc{};
            bloomPc.offset = 0;
            bloomPc.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bloomPc.size = sizeof(float) * 2; // texel size for downsample/upsample

            VkDescriptorSetLayout bloomSets[] = {ctx_.bloomSampleSetLayout()}; // 1 sampler
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 1;
            li.pSetLayouts = bloomSets;
            li.pushConstantRangeCount = 1;
            li.pPushConstantRanges = &bloomPc;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &bloomLayout_));
        }

        {
            VkDescriptorSetLayout combineSet[] = {ctx_.combineSetLayout()}; // 2 samplers
            VkPipelineLayoutCreateInfo li{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            li.setLayoutCount = 1;
            li.pSetLayouts = combineSet;
            li.pushConstantRangeCount = 0;
            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &li, nullptr, &combineLayout_));
        }

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

            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &layoutInfo, nullptr, &pbrLayout_));
        }

        const auto attrs = vertexAttributes();
        const auto shadowAttrs = shadowVertexAttributes();
        const auto bind = vertexBinding();

        const VertexInputDesc meshVertexInput{
            .attributes = {attrs.begin(), attrs.end()},
            .binding = bind,
        };

        const VertexInputDesc shadowVertexInput{
            .attributes = {shadowAttrs.begin(), shadowAttrs.end()},
            .binding = bind,
        };

        const VkFormat hdrFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        const VkFormat depthFormat = swapchain_.depthFormat();
        const VkSampleCountFlagBits samples = ctx_.getSampleCount(VK_SAMPLE_COUNT_4_BIT);

        pbrOpaqueHandle_ = pipelineReg_.create("pbr_opaque",
                            {
                                .layout = pbrLayout_,
                                .desc =
                                    {
                                        .vertShader = "pbr.vert",
                                        .fragShader = "pbr.frag",
                                        .vertexInput = meshVertexInput,
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_BACK_BIT,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = true,
                                                .write = true,
                                                .format = depthFormat,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                        .samples = samples,
                                    },
                            });

        pbrBlendHandle_ = pipelineReg_.create(
            "pbr_blend",
                            {
                                .layout = pbrLayout_,
                                .desc =
                                    {
                                        .vertShader = "pbr.vert",
                                        .fragShader = "pbr.frag",
                                        .vertexInput = meshVertexInput,
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_BACK_BIT,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = true,
                                                .write = false,
                                                .format = depthFormat,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = true,
                                            },
                                        .samples = samples,
                                    },
                            });

        wireframeHandle_ = pipelineReg_.create(
            "pbr_wireframe",
                            {
                                .layout = pbrLayout_,
                                .desc =
                                    {
                                        .vertShader = "pbr.vert",
                                        .fragShader = "pbr.frag",
                                        .vertexInput = meshVertexInput,
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_LINE,
                                                .cullMode = VK_CULL_MODE_BACK_BIT,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = true,
                                                .write = true,
                                                .format = depthFormat,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                        .samples = samples,
                                    },
                            });

        skyboxHandle_ = pipelineReg_.create(
            "skybox",
                            {
                                .layout = pipelineLayout_,
                                .desc =
                                    {
                                        .vertShader = "skybox.vert",
                                        .fragShader = "skybox.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = true,
                                                .write = false,
                                                .compareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
                                                .format = depthFormat,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                        .samples = samples,
                                    },
                            });

        irradianceHandle_ = pipelineReg_.create(
            "irradiance",
                            {
                                .layout = irradianceLayout_,
                                .desc =
                                    {
                                        .vertShader = "irradiance.vert",
                                        .fragShader = "irradiance.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
                            });

        brdfHandle_ = pipelineReg_.create("brdf_lut",
                            {
                                .layout = brdfLutLayout_,
                                .desc =
                                    {
                                        .vertShader = "brdf_lut.vert",
                                        .fragShader = "brdf_lut.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
                            });

        prefilterHandle_ = pipelineReg_.create(
            "prefilter",
                            {
                                .layout = prefilterLayout_,
                                .desc =
                                    {
                                        .vertShader = "prefilter.vert",
                                        .fragShader = "prefilter.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
                            });

        shadowHandle_ = pipelineReg_.create(
            "shadow",
                            {
                                .layout = shadowLayout_,
                                .desc =
                                    {
                                        .vertShader = "shadow.vert",
                                        .fragShader = "shadow.frag",
                                        .vertexInput = shadowVertexInput,
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = true,
                                                .write = true,
                                                .bias = true,
                                                .format = depthFormat,
                                            },
                                    },
                            });

        softThresholdHandle_ = pipelineReg_.create(
            "softThreshold",
                            {
                                .layout = thresholdLayout_,
                                .desc =
                                    {
                                        .vertShader = "softThreshold.vert",
                                        .fragShader = "softThreshold.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
                            });

        downsampleHandle_ = pipelineReg_.create(
            "downsample",
                            {
                                .layout = bloomLayout_,
                                .desc =
                                    {
                                        .vertShader = "postProcess.vert",
                                        .fragShader = "downsample.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
                            });

        upsampleHandle_ = pipelineReg_.create(
            "upsample",
                            {
                                .layout = bloomLayout_,
                                .desc =
                                    {
                                        .vertShader = "postProcess.vert",
                                        .fragShader = "upsample.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = true,
                                            },
                                    },
                            });

        combineHandle_ = pipelineReg_.create("combinePostProcess",
                            {
                                .layout = combineLayout_,
                                .desc =
                                    {
                                        .vertShader = "combinePostProcess.vert",
                                        .fragShader = "combinePostProcess.frag",
                                        .raster =
                                            {
                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                .cullMode = VK_CULL_MODE_NONE,
                                                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            },
                                        .depth =
                                            {
                                                .test = false,
                                                .write = false,
                                            },
                                        .color =
                                            {
                                                .format = hdrFormat,
                                                .blending = false,
                                            },
                                    },
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

                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(irradianceHandle_));
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

            vkCmdBindPipeline(
                cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(brdfHandle_));

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

                    vkCmdBindPipeline(
                        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(prefilterHandle_));
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
                                       const FrameRenderData& renderData,
                                       CRGTextureHandle& shadowHandle)
    {
        auto& target = frames_[currentFrame_].shadowTarget;
        const auto& items = renderData.items;

        struct ShadowData {
            CRGTextureHandle output;
        };
        renderGraph_.addPass<ShadowData>(
            "ShadowMap",
            [&](CRGBuilder& builder, ShadowData& data) {
                data.output = builder.write(shadowHandle);
            },
            [&, order](VkCommandBuffer cmd, const ShadowData& data, const CRGResources& resources) {
                // body is your existing shadowMapping() code, minus its own imageBarrier call —
                // the graph now issues that barrier automatically before this lambda runs
                VkRenderingAttachmentInfo depth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
                depth.imageView = resources.attachmentView(data.output, 0);
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

                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineReg_.get(shadowHandle_));

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
            });
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
        colors[ImGuiCol_DockingPreview] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_DockingEmptyBg] = bg;
        colors[ImGuiCol_Tab] = bgLight;
        colors[ImGuiCol_TabHovered] = accentHover;
        colors[ImGuiCol_TabSelected] = accent;
        colors[ImGuiCol_TabDimmed] = bgLight;
        colors[ImGuiCol_TabSelectedOverline] = accent;
        colors[ImGuiCol_TabDimmedSelected] = bgLight;
        colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
        colors[ImGuiCol_TabActive] = bgLight;
        // colors[ImGuiCol_TabUnfocused] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
        // colors[ImGuiCol_TabUnfocusedActive] = ImVec4(accent.x, accent.y, accent.z, 0.5f);
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

        style.GrabMinSize = 12.0f;
        style.ScrollbarSize = 12.0f;
    }

    bool VulkanRenderer::initializeUI()
    {
        ImGui::CreateContext();
        // TODO: not ideal, i dont want glfw here at all
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

        // make ImGui not look terrible
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
        uiColorAttachment.imageView = imageView;
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

    void VulkanRenderer::recompileShaders() 
    {
        pipelineReg_.reloadAll();
    }
} // namespace chai::gfx