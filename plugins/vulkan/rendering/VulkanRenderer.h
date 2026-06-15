/**
 * @file VulkanRenderer.h
 */
#pragma once
#include <Rendering/IRenderer.h>
#include "../VulkanContext.h"
#include "../Swapchain.h"
#include "../RenderTargetView.h"
#include <vulkan/vulkan.h>
#include <Plugin/ServiceLocator.h>
 
#include <array>
#include <cstdint>
#include "../GpuResources.h"
#include "../MaterialFactory.h"

namespace chai
{
    class IWindow;
}

namespace chai::gfx
{
    /**
     * @brief Concrete vulkan implementation of the Renderer interface.
     */
	class VulkanRenderer : public IRenderer
	{
    public:
        VulkanRenderer(chai::IWindow& window,
                       std::shared_ptr<AssetCache<Mesh>> meshCache,
                       std::shared_ptr<AssetCache<Texture>> texCache,
                       std::shared_ptr<AssetCache<Material>> matCache,
                       VulkanContext& context);
        ~VulkanRenderer() override;

        void renderFrame(const FrameRenderData& renderData) override;
        void onResize(int width, int height) override;
        void waitIdle() override;

    private:
        /**
         * @brief Per frame resources, double buffering
         */
        struct FrameData {
            VkCommandBuffer cmd = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            VkFence inFlight = VK_NULL_HANDLE;

            VkBuffer cameraBuffer = VK_NULL_HANDLE;
            VmaAllocation cameraAlloc = VK_NULL_HANDLE;
            void* cameraMapped = nullptr;
            VkDescriptorSet cameraSet = VK_NULL_HANDLE;

            VkDescriptorSet lightSet = VK_NULL_HANDLE;
            void* lightMapped = nullptr;
            VkBuffer lightBuffer = VK_NULL_HANDLE;
            VmaAllocation lightAlloc = VK_NULL_HANDLE;
        };
        static constexpr uint32_t kFramesInFlight = 2;

        void init();

        void recreateSwapchain();

        /**
         * @brief The actual drawing. I dont want this function to know anything about
         * the scene objects or graph. It also only renders to the target view, NOT directly
         * to the swapchain. This will make multiple view easier later...i think
         */
        void renderScene(VkCommandBuffer cmd,
                         const RenderTargetView& view,
                         const FrameRenderData& renderData);
        VkFenceCreateInfo fenceCreate(VkFenceCreateFlags flags = 0);
        VkSemaphoreCreateInfo semaphoreCreate(VkSemaphoreCreateFlags flags = 0);
        void setupPipelines();

        chai::IWindow& window_;
        VulkanContext& ctx_;
        Swapchain swapchain_;

        VkCommandPool cmdPool_ = VK_NULL_HANDLE;
        std::array<FrameData, kFramesInFlight> frames_{};
        uint32_t currentFrame_ = 0;
        bool needsResize_ = false;

        //Resources
        std::shared_ptr<AssetCache<Mesh>> meshCache_;
        std::shared_ptr<AssetCache<Texture>> texCache_;
        std::shared_ptr<AssetCache<Material>> materialCache_;

        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkPipeline pbrPipeline_ = VK_NULL_HANDLE;
    };

    //dont leave this here forever
    struct PushConstants {
        math::Mat4 model;
        math::Vec4 color;
    };
}