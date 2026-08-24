/**
 * @file ViewportRegistry.h
 */
#pragma once
#include "../VulkanCommon.h"
#include "../renderer/VulkanContext.h"
#include "../resources/VulkanViewport.h"
#include "../swapchain/Swapchain.h"

#include <Rendering/Viewport.h>

namespace chai::gfx
{
    /**
     * @brief Concrete implementation to manage a vulkan representation of a viewport.
     * The application uses this registry to manage the lifecycle of the viewport
     */
    class ViewportRegistry : public IViewportRegistry
    {
    public:
        explicit ViewportRegistry(VulkanContext& ctx);
        ~ViewportRegistry() = default;

        void init(const Swapchain& swapchain);
        void shutdown();
        void tick(uint32_t currentFrame);
        void forEachViewport(std::function<void(Viewport&)> callback);

        std::optional<Viewport> getFocusedViewport();

        /**
         * @brief This should be called every frame to update any viewport that has called
         * requestViewportResize()
         */
        void applyPendingViewportResizes();

        ViewportHandle addViewport(const std::string& id, uint64_t cameraViewIndex) override;
        void removeViewport(ViewportHandle handle) override;
        uint64_t getViewportTextureId(ViewportHandle handle) const override;
        void setViewportHovered(ViewportHandle handle, bool hovered) override;
        math::Vec2 getViewportExtent(ViewportHandle handle) const override;

        /**
         * @brief Does not immediately resize the viewport. This will resize when its safe to sync
         */
        void requestViewportResize(ViewportHandle handle, uint32_t width, uint32_t height) override;

        void setViewportWireframe(ViewportHandle handle, bool enabled) override;
        bool isViewportWireframe(ViewportHandle handle) const override;
        void setViewportShadingMode(ViewportHandle handle, ViewportShadingMode mode) override;
        ViewportShadingMode getViewportShadingMode(ViewportHandle handle) const override;

        void recreateViewportTarget(Viewport& viewport, uint32_t frameIndex);
        ViewportTarget createViewportTarget(VkExtent2D extent);

    private:
        bool isValidHandle(ViewportHandle handle) const;
        Viewport* getViewport(ViewportHandle handle) const;

        Viewport createViewport(const std::string& id, uint64_t cameraViewId);
        void destroyViewport(Viewport& vp);
        void createCameraUBO(VkBuffer& buffer,
                             VmaAllocation& alloc,
                             void*& mapped,
                             VkDescriptorSet& set);

        std::vector<ViewportSlot> viewportSlots_;
        std::vector<uint32_t> freeViewportSlots_;
        uint32_t currentFrame_ = 0;
        VulkanContext* ctx_ = nullptr;
        const Swapchain* swapchain_ = nullptr;
    };
} // namespace chai::gfx