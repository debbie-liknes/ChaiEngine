#pragma once
#include "../commands/ImageTransition.h"
#include "../renderer/VulkanContext.h"
#include "../resources/RenderTargetView.h"
#include "CRGBuilder.h"
#include "CRGDescriptors.h"
#include "CRGPass.h"

namespace chai::gfx
{
    class ChaiRenderGraph
    {
    public:
        explicit ChaiRenderGraph(VulkanContext& ctx);
        ~ChaiRenderGraph();

        CRGTextureHandle
        importTexture(const std::string& name, RenderTargetView& view, ImageState state);
        CRGTextureHandle createTexture(const std::string& name, const CRGTextureDesc& desc);

        VkImage resolvedImage(CRGTextureHandle handle) const;
        VkExtent2D resolvedExtent(CRGTextureHandle handle, uint32_t mip = 0) const;

        //helps with tracking my blitting woes
        void
        markExternalState(CRGTextureHandle handle, ImageState state, uint32_t mip = 0)
        {
            CRGTexture& tex = textures_[handle.index];
            tex.mipStates[mip] = state;
        }

        VkImageView resolvedAttachmentView(CRGTextureHandle handle,
                                                            uint32_t mip) const
        {
            const CRGTexture& tex = textures_[handle.index];
            return tex.isImported ? tex.importedTarget->colorView : tex.target.renderViews[mip];
        }

        VkImageView resolvedView(CRGTextureHandle handle, uint32_t mip) const
        {
            const CRGTexture& tex = textures_[handle.index];
            return tex.isImported ? tex.importedTarget->colorView
                                  : tex.target.view; // full-range sampling view
        }

        template <typename PassData, typename SetupFn, typename ExecuteFn>
        PassData& addPass(const std::string& name, SetupFn&& setup, ExecuteFn&& execute)
        {
            auto pass = std::make_unique<CRGPass<PassData>>();
            pass->name = name;

            PassData* dataPtr = &pass->data;
            pass->executeFn = [dataPtr, execute = std::forward<ExecuteFn>(execute)](
                                  CRGResources& res, VkCommandBuffer cmd) {
                execute(cmd, *dataPtr, res);
            };

            PassData& ref = pass->data;
            CRGBuilder builder(*this, uint32_t(passes_.size()));
            passes_.push_back(std::move(pass));
            setup(builder, ref);

            return ref;
        }

        void clear();
        void compile();
        void execute(VkCommandBuffer cmd);

    private:
        friend class CRGBuilder;
        friend class CRGResources;
        VulkanContext& ctx_;
        std::vector<std::unique_ptr<CRGPassBase>> passes_;
        std::vector<CRGTexture> textures_;
        std::vector<uint32_t> executionOrder_;
        std::unordered_map<uint32_t, std::vector<CRGBarrier>> barrierPlan_;

        std::vector<uint32_t> topologicalSort(std::vector<std::unique_ptr<CRGPassBase>>& passes);
        void computeBarriers(const std::vector<uint32_t>& order, std::vector<CRGTexture>& textures);
        void buildAdjacencyList(std::vector<std::unique_ptr<CRGPassBase>>& passes,
                                std::unordered_map<uint32_t, std::vector<uint32_t>>& adjList,
                                std::vector<uint32_t>& inDegree);
        CRGTexture buildTexture(const std::string& name, const CRGTextureDesc& desc);
    };
} // namespace chai::gfx