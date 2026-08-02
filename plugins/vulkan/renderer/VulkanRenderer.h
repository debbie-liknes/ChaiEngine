/**
 * @file VulkanRenderer.h
 */
#pragma once
#include <Rendering/IRenderer.h>

#include "VulkanContext.h"
#include "../swapchain/Swapchain.h"
#include "../resources/GpuResources.h"
#include "../resources/MaterialFactory.h"
#include "../resources/RenderTargetView.h"
#include "../resources/VulkanRenderTarget.h"
#include "../VulkanCommon.h"

#include <Plugin/ServiceLocator.h>

#include <vulkan/vulkan.h>
#include <array>
#include <cstdint>
#include "../resources/TextureRegistry.h"
#include "../resources/ModelRegistry.h"
#include <UI/Editor/InternalChaiUI.h>
#include "../utils/GpuProfiler.h"
#include "../registries/ViewportRegistry.h"

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

        //setup
        void init();
        VkFenceCreateInfo fenceCreate(VkFenceCreateFlags flags = 0);
        VkSemaphoreCreateInfo semaphoreCreate(VkSemaphoreCreateFlags flags = 0);
        void setupPipelines();
        void ensureSkyboxSet(FrameData& frame, const GpuTexture& cube, Handle<Texture> handle);
        void bakeIrradiance(const GpuTexture& envCube);
        void bakeBrdfLut();
        void bakePrefilter(const GpuTexture& envCube);
        void writeEnvironmentSet(const GpuTexture& skybox);
        void shadowMapping(VkCommandBuffer cmd, const std::vector<uint32_t>&, const FrameRenderData&);

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

        //Caches
        std::shared_ptr<AssetCache<Mesh>> meshCache_;
        std::shared_ptr<AssetCache<Texture>> texCache_;
        std::shared_ptr<AssetCache<Material>> materialCache_;
        std::shared_ptr<ModelRegistry> modelReg_;

        //pipelines and layouts
        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkPipeline pbrPipeline_ = VK_NULL_HANDLE;
        VkPipeline pbrBlendPipeline_ = VK_NULL_HANDLE;
        VkPipeline pbrWireframePipeline_ = VK_NULL_HANDLE;
        VkPipeline skyboxPipeline_ = VK_NULL_HANDLE;

        //IBL
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

        //shadows
        VkPipeline shadowPipeline_ = VK_NULL_HANDLE;
        VkPipelineLayout shadowLayout_ = VK_NULL_HANDLE;

        VkDescriptorSet skyboxSet_ = VK_NULL_HANDLE;
        Handle<Texture> skyboxCube_;
    };

    //TODO: dont leave this here forever
    struct PushConstants {
        math::Mat4 model;
        int shadingMode = 0; // 0=Lit, 1=Normals, 2=UV, 3=Roughness, 4=Metallic, 5=AO
    };
}
