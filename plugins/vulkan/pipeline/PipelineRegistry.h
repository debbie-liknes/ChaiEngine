#pragma once
#include "../VulkanCommon.h"
#include "../renderer/VulkanContext.h"
#include "PipelineKey.h"
#include "PipelineBuilder.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <future>
#include <array>

namespace chai::gfx
{
    struct PipelineCreateInfo {
        VkPipelineLayout layout;
        PipelineDesc desc;
    };

    struct PipelineEntry {
        std::string name;
        PipelineKey key;
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    class PipelineRegistry
    {
    public:
        explicit PipelineRegistry(VulkanContext& ctx);

        PipelineHandle create(std::string name, PipelineKey key);
        VkPipeline get(const PipelineHandle& key);

        void reloadAll();
        void reloadAllAsync();
        void destroyAll();

        void prcoessPendingBuilds(uint32_t currentFrameIndex);
        void collectGarbage(uint32_t frameIndex);

    private:
        VkPipeline build(const PipelineEntry& entry);

        VulkanContext& ctx_;
        std::vector<PipelineEntry> entries_;
        bool reloadInProgress_ = false;
        std::future<std::vector<std::pair<PipelineHandle, VkPipeline>>> reloadFuture_;
        std::array<std::vector<VkPipeline>, kFramesInFlight> deferredDelete_;
    };
}