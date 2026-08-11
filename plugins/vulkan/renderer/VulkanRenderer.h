/**
 * @file VulkanRenderer.h
 */
#pragma once
#include "../VulkanCommon.h"
#include "../pipeline/PipelineCache.h"
#include "../registries/ViewportRegistry.h"
#include "../renderGraph/ChaiRenderGraph.h"
#include "../resources/GpuResources.h"
#include "../resources/MaterialFactory.h"
#include "../resources/ModelRegistry.h"
#include "../resources/RenderTargetView.h"
#include "../resources/TextureRegistry.h"
#include "../resources/VulkanRenderTarget.h"
#include "../swapchain/Swapchain.h"
#include "../utils/GpuProfiler.h"
#include "VulkanContext.h"

#include <Plugin/ServiceLocator.h>
#include <Rendering/IRenderer.h>
#include <UI/Editor/InternalChaiUI.h>
#include <array>
#include <cstdint>
#include <vulkan/vulkan.h>

namespace chai
{
    class IWindow;
}

namespace chai::gfx
{
    struct PassStats {
        uint32_t drawCalls;
        float gpuTimeMs;
    };

    struct VulkanStats {
        float gpuTimeMs;
        PassStats mainPass;
        PassStats shadowPass;

        void clear()
        {
            mainPass.drawCalls = 0;
            shadowPass.drawCalls = 0;
        }
    };

    /**
     * @brief Concrete vulkan implementation of the Renderer interface.
     */
    class VulkanRenderer : public IRenderer, public ui::IInternalChaiUI
    {
    public:
        VulkanRenderer(chai::IWindow& window,
                       std::shared_ptr<AssetCache<Mesh>> meshCache,
                       std::shared_ptr<AssetCache<Texture>> texCache,
                       std::shared_ptr<AssetCache<Material>> matCache,
                       std::shared_ptr<ModelRegistry> texReg,
                       std::shared_ptr<ViewportRegistry> viewportReg,
                       VulkanContext& context,
                       chai::ServiceLocator& locator);
        ~VulkanRenderer() override;

        void renderFrame(const FrameRenderData& renderData) override;
        void onResize(int width, int height) override;
        void waitIdle() override;

        bool initializeUI() override;
        void shutdownUI() override;

        void startFrame() override;
        void endFrame() override;

        VulkanStats& getStats() { return stats_; }

    private:
        /**
         * @brief Per frame resources, double buffering
         */
        struct FrameData {
            VkCommandBuffer cmd = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            VkFence inFlight = VK_NULL_HANDLE;

            VkDescriptorSet lightSet = VK_NULL_HANDLE;
            VkDescriptorSet bloomThresholdSet = VK_NULL_HANDLE;
            VkDescriptorSet combineSet = VK_NULL_HANDLE;
            std::vector<VkDescriptorSet> bloomSampleSets;
            void* lightMapped = nullptr;
            VkBuffer lightBuffer = VK_NULL_HANDLE;
            VmaAllocation lightAlloc = VK_NULL_HANDLE;

            RenderTarget shadowTarget{};
        };

        void recreateSwapchain();

        /**
         * @brief The actual drawing. I dont want this function to know anything about
         * the scene objects or graph. It also only renders to the target view, NOT directly
         * to the swapchain. This will make multiple view easier later...i think
         */
        void renderScene(VkCommandBuffer cmd,
                         const RenderTargetView& view,
                         const FrameRenderData& renderData,
                         const std::vector<uint32_t>&,
                         VkDescriptorSet,
                         ViewportShadingMode,
                         bool wireframe);

        // setup
        void init();
        VkFenceCreateInfo fenceCreate(VkFenceCreateFlags flags = 0);
        VkSemaphoreCreateInfo semaphoreCreate(VkSemaphoreCreateFlags flags = 0);
        void setupPipelines();
        void setupThreshold(VkImageView view);
        void setupCombine(VkImageView view, VkImageView bloomView);
        void bloomPass(ChaiRenderGraph& renderGraph,
                       RenderTargetView& scene,
                       CRGTextureHandle& sceneHandle,
                       CRGTextureHandle& bloomChain);
        void combinePass(ChaiRenderGraph& renderGraph,
                         RenderTargetView& target,
                         CRGTextureHandle& sceneHandle,
                         CRGTextureHandle& bloomChain,
                         CRGTextureHandle& combineTarget);
        void blitCombineToViewport(VkCommandBuffer cmd,
                                   const RenderTargetView& view,
                                   VkImage combineImage,
                                   VkExtent2D combineExtent,
            bool everRendered);
        void ensureSkyboxSet(FrameData& frame, const GpuTexture& cube, Handle<Texture> handle);
        void bakeIrradiance(const GpuTexture& envCube);
        void bakeBrdfLut();
        void bakePrefilter(const GpuTexture& envCube);
        void writeEnvironmentSet(const GpuTexture& skybox);
        void shadowMapping(VkCommandBuffer cmd,
                           const std::vector<uint32_t>&,
                           const FrameRenderData&,
                           CRGTextureHandle&);

        void beginUIFrame();
        void endUIFrame();
        void renderUI(VkCommandBuffer cmd, VkImageView imageView);

        VulkanStats stats_{};
        GpuProfiler profiler_{};
        ViewportRegistry& viewportReg_;

        chai::IWindow& window_;
        VulkanContext& ctx_;
        Swapchain swapchain_;
        chai::ServiceLocator& locator_;

        VkCommandPool cmdPool_ = VK_NULL_HANDLE;
        std::array<FrameData, kFramesInFlight> frames_{};
        uint32_t currentFrame_ = 0;
        bool needsResize_ = false;

        // Caches
        std::shared_ptr<AssetCache<Mesh>> meshCache_;
        std::shared_ptr<AssetCache<Texture>> texCache_;
        std::shared_ptr<AssetCache<Material>> materialCache_;
        std::shared_ptr<ModelRegistry> modelReg_;

        PipelineCache pipelineCache_;
        ChaiRenderGraph renderGraph_;

        // pipelines and layouts
        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkPipeline skyboxPipeline_ = VK_NULL_HANDLE;

        // IBL
        VkPipeline irradiancePipeline_ = VK_NULL_HANDLE;
        RenderTarget irradianceTarget_{};
        VkPipelineLayout irradianceLayout_ = VK_NULL_HANDLE;
        VkDescriptorSet environmentSet_ = VK_NULL_HANDLE;

        VkPipeline brdfLutPipeline_ = VK_NULL_HANDLE;
        RenderTarget brdfLut_{};
        VkPipelineLayout brdfLutLayout_ = VK_NULL_HANDLE;
        bool brdfBaked_ = false;

        RenderTarget prefilterTarget_{};
        VkPipeline prefilterPipeline_ = VK_NULL_HANDLE;
        VkPipelineLayout prefilterLayout_ = VK_NULL_HANDLE;
        VkDescriptorSet prefilterSet_ = VK_NULL_HANDLE;
        bool iblBaked_ = false;

        // shadows
        VkPipeline shadowPipeline_ = VK_NULL_HANDLE;
        VkPipelineLayout shadowLayout_ = VK_NULL_HANDLE;

        VkDescriptorSet skyboxSet_ = VK_NULL_HANDLE;
        Handle<Texture> skyboxCube_;

        // post processing
        VkPipeline thresholdPipeline_ = VK_NULL_HANDLE;
        VkPipeline downsamplePipeline_ = VK_NULL_HANDLE;
        VkPipeline upsamplePipeline_ = VK_NULL_HANDLE;
        VkPipeline combinePipeline_ = VK_NULL_HANDLE;
        VkPipelineLayout thresholdLayout_ = VK_NULL_HANDLE;
        VkPipelineLayout combineLayout_ = VK_NULL_HANDLE;
        VkPipelineLayout bloomLayout_ = VK_NULL_HANDLE;
    };
} // namespace chai::gfx
