/**
 * @file VulkanRenderer.h
 */
#pragma once
#include <Renderer.h>
#include "../VulkanContext.h"
#include "../Swapchain.h"
#include "../RenderTargetView.h"
#include <vulkan/vulkan.h>
 
#include <array>
#include <cstdint>

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
        explicit VulkanRenderer(chai::IWindow& window);
        ~VulkanRenderer() override;

        void renderFrame() override;
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
        };
        static constexpr uint32_t kFramesInFlight = 2;

        void init();

        void recreateSwapchain();

        /**
         * @brief The actual drawing. I dont want this function to know anything about
         * the scene objects or graph. It also only renders to the target view, NOT directly
         * to the swapchain. This will make multiple view easier later...i think
         */
        void renderScene(VkCommandBuffer cmd, const RenderTargetView& view);
        VkFenceCreateInfo fenceCreate(VkFenceCreateFlags flags = 0);
        VkSemaphoreCreateInfo semaphoreCreate(VkSemaphoreCreateFlags flags = 0);

        chai::IWindow& window_;
        VulkanContext ctx_;
        Swapchain swapchain_;

        VkCommandPool cmdPool_ = VK_NULL_HANDLE;
        std::array<FrameData, kFramesInFlight> frames_{};
        uint32_t currentFrame_ = 0;
        bool needsResize_ = false;

        //Porbably dont want this long term, just for hello triangle (vulkan edition!!)
        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkPipeline trianglePipeline_ = VK_NULL_HANDLE;
    };
}