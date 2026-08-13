#pragma once
#include "../VulkanCommon.h"
#include "../renderer/VulkanContext.h"
#include "PipelineKey.h"
#include "PipelineBuilder.h"
#include <string>
#include <functional>
#include <unordered_map>

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
        void destroyAll();

    private:
        VkPipeline build(const PipelineEntry& entry);

        VulkanContext& ctx_;
        std::vector<PipelineEntry> entries_;
    };
}