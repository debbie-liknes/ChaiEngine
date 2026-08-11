#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <array>
#include "PipelineKey.h"
#include "../renderer/VulkanContext.h"

namespace chai::gfx
{
	class PipelineCache
	{
    public:
        PipelineCache(VulkanContext& ctx, VkFormat colorFormat, VkFormat depthFormat);
        ~PipelineCache() = default;

		VkPipeline getOrCreate(const PipelineKey& key);
        void destroyAll();

	private:
        std::unordered_map<PipelineKey, VkPipeline, PipelineKeyHash> pipelines_;
        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkVertexInputBindingDescription binding_;
        std::array<VkVertexInputAttributeDescription, 4> attrs_;
        VkFormat colorFormat_;
        VkFormat depthFormat_;

		VkPipeline createPipeline(const PipelineKey& key);
        VulkanContext& ctx_;
	};
}