/**
 * @file PipelineRegistry.h
 */
#pragma once
#include "../VulkanCommon.h"
#include "../renderer/VulkanContext.h"
#include "PipelineBuilder.h"
#include "PipelineKey.h"

#include <array>
#include <functional>
#include <future>
#include <string>
#include <unordered_map>

namespace chai::gfx
{
    /**
     * @brief Named pipeline with key
     */
    struct PipelineEntry {
        std::string name;
        PipelineKey key;
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    /**
     * @brief Pipeline that needs to be rebuilt. The future will become ready when the pipeline is
     * done building
     */
    struct PendingPipelineBuild {
        PipelineHandle handle;
        std::future<VkPipeline> future;
    };

    /**
     * @brief Class for managing pipelines. Shaders are compiled during runtime, and can be done
     * synchronously or asynchronously.
     */
    class PipelineRegistry
    {
    public:
        explicit PipelineRegistry(VulkanContext& ctx);

        /**
         * @brief Synchronous creation of pipelines. Will block the cpu thread for a potentially
         * expensive compile operation
         */
        PipelineHandle create(std::string name, PipelineKey key);

        /**
         * @brief Asynchronous creation of pipelines. Will not block the cpu, but using get with
         * the returned PipelineHandle will get a VK_NULL_HANDLE until the shader compilation and
         * pipeline building is complete
         */
        PipelineHandle createAsync(std::string name, PipelineKey key);

        /**
         * @returns VK_NULL_HANDLE if the pipeline is not ready
         */
        VkPipeline get(const PipelineHandle& key);

        /**
         * @brief Synchronous reload of all pipelines
         * @note Please use this for debugging, NOT for production use
         */
        void reloadAll();

        /**
         * @brief Async reload of all pipelines
         */
        void reloadAllAsync();

        /**
         * @brief Immediately destroys all pipelines
         */
        void destroyAll();

        /**
         * @brief Call this once per frame when the renderer with this frame index, after the fence
         * wait. Processes any pending pipelines, and removes from pending list.
         */
        void processPendingBuilds(uint32_t currentFrameIndex);

        /**
         * @brief Call this after processPendingBuilds once per frame. Deletes pipelines marked for
         * deferred deletion.
         */
        void collectGarbage(uint32_t frameIndex);

    private:
        VkPipeline build(const PipelineEntry& entry);
        void enqueueBuild(PipelineHandle handle, PipelineEntry snapshot);

        VulkanContext& ctx_;
        std::vector<PipelineEntry> entries_;
        std::vector<PendingPipelineBuild> pending_;
        std::array<std::vector<VkPipeline>, kFramesInFlight> deferredDelete_;

        std::atomic<bool> running_{true};
    };
} // namespace chai::gfx