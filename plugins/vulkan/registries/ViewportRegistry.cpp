#include "ViewportRegistry.h"
#include "../utils/VkCheck.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include <Rendering/CameraData.h>

namespace chai::gfx
{
    ViewportRegistry::ViewportRegistry(VulkanContext& ctx)
    {
        ctx_ = &ctx;
    }

    void ViewportRegistry::init(const Swapchain& swapchain)
    {
        swapchain_ = &swapchain;
    }

    void ViewportRegistry::shutdown()
    {
        for (auto& slot : viewportSlots_) {
            if (slot.alive)
                destroyViewport(slot.viewport);
        }

        ctx_ = nullptr;
        swapchain_ = nullptr;
    }

    void ViewportRegistry::tick(uint32_t currentFrame)
    {
        currentFrame_ = currentFrame;
    }

    void ViewportRegistry::forEachViewport(std::function<void(Viewport&)> callback)
    {
        for (auto& slot : viewportSlots_) {
            if (slot.alive)
            {
                callback(slot.viewport);
            }
        }
    }

    std::optional<Viewport> ViewportRegistry::getFocusedViewport()
    {
        for (auto& slot : viewportSlots_) {
            if (!slot.alive)
                continue;
            if (slot.viewport.hovered)
                return slot.viewport;
        }
        return std::nullopt;
    }

    void ViewportRegistry::applyPendingViewportResizes()
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

    ViewportTarget ViewportRegistry::createViewportTarget(VkExtent2D extent)
    {
        ViewportTarget target;
        target.view.extent = extent;
        target.view.colorFormat = swapchain_->format();
        target.view.depthFormat = swapchain_->depthFormat();
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
        colorInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                          VK_IMAGE_USAGE_SAMPLED_BIT;

        VmaAllocationCreateInfo colorAlloc{};
        colorAlloc.usage = VMA_MEMORY_USAGE_AUTO;

        VK_CHECK(vmaCreateImage(ctx_->allocator(),
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
        VK_CHECK(vkCreateImageView(ctx_->device(), &colorViewInfo, nullptr, &target.view.colorView));

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

        VK_CHECK(vmaCreateImage(ctx_->allocator(),
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
        VK_CHECK(vkCreateImageView(ctx_->device(), &depthViewInfo, nullptr, &target.view.depthView));

        return target;
    }

    void ViewportRegistry::recreateViewportTarget(Viewport& viewport,
                                                  uint32_t frameIndex)
    {
        ViewportTarget& target = viewport.targets[frameIndex];

        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)target.imguiTextureId);
        target.destroy(*ctx_);

        target = createViewportTarget(viewport.pendingExtent);
        target.imguiTextureId = (ImTextureID)ImGui_ImplVulkan_AddTexture(
            ctx_->linearSampler(), target.view.colorView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        viewport.everRendered[frameIndex] = false; // fresh image, back to Undefined
    }

    void ViewportRegistry::createCameraUBO(VkBuffer& buffer,
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
        VK_CHECK(vmaCreateBuffer(ctx_->allocator(), &bufInfo, &aci, &buffer, &alloc, &allocInfo));
        mapped = allocInfo.pMappedData;

        VkDescriptorSetLayout camLayout = ctx_->cameraSetLayout();
        VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        dsai.descriptorPool = ctx_->descriptorPool();
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &camLayout;
        VK_CHECK(vkAllocateDescriptorSets(ctx_->device(), &dsai, &set));

        VkDescriptorBufferInfo dbi{buffer, 0, sizeof(CameraData)};
        VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = set;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &dbi;
        vkUpdateDescriptorSets(ctx_->device(), 1, &write, 0, nullptr);
    }

    Viewport ViewportRegistry::createViewport(const std::string& id,
                                              uint32_t cameraViewId)
    {
        Viewport vp;
        vp.id = id;
        vp.cameraViewId = cameraViewId;

        for (int i = 0; i < kFramesInFlight; i++) {
            vp.targets[i] = createViewportTarget(swapchain_->extent());
            vp.targets[i].imguiTextureId =
                (ImTextureID)ImGui_ImplVulkan_AddTexture(ctx_->linearSampler(),
                                                         vp.targets[i].view.colorView,
                                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            createCameraUBO(
                vp.cameraBuffer[i], vp.cameraAlloc[i], vp.cameraMapped[i], vp.cameraSet[i]);
        }
        return vp;
    }

    void ViewportRegistry::destroyViewport(Viewport& vp)
    {
        vkDeviceWaitIdle(ctx_->device());

        for (int i = 0; i < kFramesInFlight; i++) {
            ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)vp.targets[i].imguiTextureId);
            vp.targets[i].destroy(*ctx_);

            vkDestroyBuffer(ctx_->device(), vp.cameraBuffer[i], nullptr);
            vmaFreeMemory(ctx_->allocator(), vp.cameraAlloc[i]);
        }
    }

    ViewportHandle ViewportRegistry::addViewport(const std::string& id,
                                                 uint32_t cameraViewId)
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
        slot.viewport = createViewport(id, cameraViewId);
        slot.alive = true;

        return ViewportHandle{index, slot.generation};
    }

    void ViewportRegistry::removeViewport(ViewportHandle handle)
    {
        if (!isValidHandle(handle))
            return;

        ViewportSlot& slot = viewportSlots_[handle.index];
        destroyViewport(slot.viewport);

        slot.alive = false;
        slot.generation++;
        freeViewportSlots_.push_back(handle.index);
    }

    bool ViewportRegistry::isValidHandle(ViewportHandle handle) const
    {
        return handle.valid() && handle.index < viewportSlots_.size() &&
               viewportSlots_[handle.index].alive &&
               viewportSlots_[handle.index].generation == handle.generation;
    }

    Viewport* ViewportRegistry::getViewport(ViewportHandle handle) const
    {
        if (isValidHandle(handle))      
            return const_cast<Viewport*>(&viewportSlots_[handle.index].viewport);

        return nullptr;
    }

    uint64_t ViewportRegistry::getViewportTextureId(ViewportHandle handle) const
    {
        auto* vp = getViewport(handle);
        return vp ? (uint64_t)vp->targets[currentFrame_].imguiTextureId : 0;
    }

    void ViewportRegistry::setViewportHovered(ViewportHandle handle, bool hovered)
    {
        if (auto* vp = getViewport(handle))
            vp->hovered = hovered;
    }

    void ViewportRegistry::requestViewportResize(ViewportHandle handle, uint32_t w, uint32_t h)
    {
        if (auto* vp = getViewport(handle)) {
            vp->pendingExtent = {w, h};
            vp->needsResize = true;
        }
    }

    void ViewportRegistry::setViewportWireframe(ViewportHandle handle, bool enabled)
    {
        if (auto* vp = getViewport(handle))
            vp->wireframe = enabled;
    }
    bool ViewportRegistry::isViewportWireframe(ViewportHandle handle) const
    {
        auto* vp = getViewport(handle);
        return vp ? vp->wireframe : false;
    }
    void ViewportRegistry::setViewportShadingMode(ViewportHandle handle, ViewportShadingMode mode)
    {
        if (auto* vp = getViewport(handle))
            vp->shadingMode = mode;
    }
    ViewportShadingMode ViewportRegistry::getViewportShadingMode(ViewportHandle handle) const
    {
        auto* vp = getViewport(handle);
        return vp ? vp->shadingMode : ViewportShadingMode::Lit;
    }
    math::Vec2 ViewportRegistry::getViewportExtent(ViewportHandle handle) const
    {
        auto* vp = getViewport(handle);
        if (!vp)
            return {0, 0};
        auto& t = vp->targets[currentFrame_];
        return {t.view.extent.width, t.view.extent.height};
    }
}