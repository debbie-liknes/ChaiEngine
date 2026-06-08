#pragma once
#include <Renderer.h>
//#include <VulkanContext.h>
#include "Swapchain.h"
#include "RenderTargetView.h"
#include <vulkan/vulkan.h>
 
#include <array>
#include <cstdint>

namespace chai
{
    class IWindow;
}

namespace chai::gfx
{
	class VulkanRenderer : public IRenderer
	{
    public:
        explicit VulkanRenderer(IWindow& window);
        ~VulkanRenderer() override;

        void renderFrame() override;
        void onResize(int width, int height) override;
        void waitIdle() override;

    private:
        // Per-frame-in-flight resources. Double-buffering these is what lets the
        // CPU record frame N+1 while the GPU still works on frame N.
        struct FrameData {
            VkCommandBuffer cmd = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE; // acquire -> render
            VkSemaphore renderFinished = VK_NULL_HANDLE; // render  -> present
            VkFence inFlight = VK_NULL_HANDLE;           // CPU waits on this
        };
        static constexpr uint32_t kFramesInFlight = 2;

        void recreateSwapchain();

        // THE seam where actual drawing lives. For first light this is empty —
        // the clear happens via loadOp=CLEAR in renderFrame. Your triangle, then
        // meshes, go here later. Note it ONLY sees the view: it has no idea
        // whether it's drawing to the swapchain or an offscreen target. That's
        // the whole payoff.
        void renderScene(VkCommandBuffer cmd, const RenderTargetView& view);

        IWindow& window_;
        //VulkanContext ctx_;   // constructed first (everything needs it)
        //Swapchain swapchain_; // constructed from ctx_ + window extent

        VkCommandPool cmdPool_ = VK_NULL_HANDLE;
        std::array<FrameData, kFramesInFlight> frames_{};
        uint32_t currentFrame_ = 0;
        bool needsResize_ = false;
    };
}