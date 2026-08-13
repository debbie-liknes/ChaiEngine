#include "PipelineRegistry.h"

#include "PipelineHelpers.h"

namespace chai::gfx
{
    PipelineRegistry::PipelineRegistry(VulkanContext& ctx) : ctx_(ctx) {}

    PipelineHandle PipelineRegistry::create(std::string name, PipelineKey key)
    {
        PipelineEntry entry = {name, key};
        entry.pipeline = build(entry);
        PipelineHandle handle{.index = static_cast<uint32_t>(entries_.size())};
        auto& inserted = entries_.emplace_back(std::move(entry));

        return handle;
    }

    void PipelineRegistry::reloadAll()
    {
        vkDeviceWaitIdle(ctx_.device());
         for (auto& entry : entries_) {
             VkPipeline newPipe = build(entry);
             if (newPipe == VK_NULL_HANDLE) {
                 CHAI_LOG_ERROR("Reload failed for '{}', keeping old pipeline", entry.name);
                 continue;
             }
             vkDestroyPipeline(ctx_.device(), entry.pipeline, nullptr);
             entry.pipeline = newPipe;
         }
    }

    void PipelineRegistry::reloadAllAsync()
    {
        if (reloadInProgress_) {
            return;
        }
        reloadInProgress_ = true;

        reloadFuture_ = std::async(std::launch::async, [this] { 
            std::vector<std::pair<PipelineHandle, VkPipeline>> results;
            for (int i = 0; i < entries_.size(); i++) {
                PipelineEntry& entry = entries_[i];
                VkPipeline newPipe = build(entry);
                PipelineHandle handle{.index = static_cast<uint32_t>(i)};
                results.push_back({handle, newPipe});
            }
            return results;
        });
    }

    void PipelineRegistry::destroyAll()
    {
         for (auto& entry : entries_)
             vkDestroyPipeline(ctx_.device(), entry.pipeline, nullptr);
         entries_.clear();
    }

    void PipelineRegistry::prcoessPendingBuilds(uint32_t currentFrameIndex)
    {
        if (!reloadInProgress_)
            return;

        if (reloadFuture_.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            return;

        for (auto& [handle, pipe] : reloadFuture_.get()) {
            PipelineEntry& entry = entries_[handle.index];
            if (pipe == VK_NULL_HANDLE) {
                CHAI_LOG_ERROR("Reload failed for '{}', keeping previous pipeline", entry.name);
                continue;
            }
            deferredDelete_[currentFrameIndex].push_back(entry.pipeline);
            entry.pipeline = pipe;
        }
        reloadInProgress_ = false;
    }

    void PipelineRegistry::collectGarbage(uint32_t frameIndex)
    {
        for (auto pipe : deferredDelete_[frameIndex])
            vkDestroyPipeline(ctx_.device(), pipe, nullptr);
        deferredDelete_[frameIndex].clear();
    }

    VkPipeline PipelineRegistry::get(const PipelineHandle& handle)
    {
        return entries_.size() > handle.index ? entries_[handle.index].pipeline : VK_NULL_HANDLE;
    }

    VkPipeline PipelineRegistry::build(const PipelineEntry& entry)
    {
        const PipelineDesc& desc = entry.key.desc;

        auto configure = [&](PipelineBuilder& builder) {
            // Vertex input
            if (!desc.vertexInput.attributes.empty()) {
                builder.setVertexInput(desc.vertexInput.attributes, desc.vertexInput.binding);
            }

            // Color
            if (desc.color.format != VK_FORMAT_UNDEFINED) {
                builder.setColorFormat(desc.color.format);
            }

            // Depth
            if (desc.depth.format != VK_FORMAT_UNDEFINED) {
                builder.setDepthFormat(desc.depth.format);
            }

            if (desc.depth.test)
                builder.enableDepthTest();
            else
                builder.disableDepthTest();

            if (desc.depth.write)
                builder.enableDepthWrite();
            else
                builder.disableDepthWrite();

            if (desc.depth.test)
                builder.setDepthOp(desc.depth.compareOp);

            if (desc.depth.bias)
                builder.enableDepthBias();

            // Rasterization
            builder.setPolygonMode(desc.raster.polygonMode)
                .setCullMode(desc.raster.cullMode, desc.raster.frontFace);

            // Blending
            if (desc.color.blending)
                builder.enableBlending();
            else
                builder.disableBlending();

            // Multisampling
            builder.setSampleCount(desc.samples);
        };

        return loadPipelineByName(
            ctx_, desc.vertShader, desc.fragShader, entry.key.layout, configure);
    }
} // namespace chai::gfx