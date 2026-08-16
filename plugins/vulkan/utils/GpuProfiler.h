/**
 * @file GpuProfiler.h
 */
#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <tracy/TracyVulkan.hpp>
#include <unordered_map>
#include <vector>

namespace chai::gfx
{
    /**
     * @brief Collects GPU stats for named regions. Also adds tracy calls for use with the tracy
     * server
     */
    class GpuProfiler
    {
    public:
        void initialize(VkDevice device,
                        VkPhysicalDevice physicalDevice,
                        VkQueue queue,
                        VkCommandBuffer setupBuff,
                        uint32_t framesInFlight);

        void shutdown(VkDevice device);

        /**
         * @brief Called when this frame slow is about to be recorded. Resets the frame start timestamp for this frame only
         */
        void beginFrame(VkCommandBuffer cmd, uint32_t frameIndex);

        /**
         * @brief Timestamp pair around a region
         */
        void beginRegion(VkCommandBuffer cmd, const std::string& name);
        void endRegion(VkCommandBuffer cmd, const std::string& name);

        /**
         * @brief Writes the frame end timestamp. Do not attempt to read results yet
         */
        void endFrame(VkCommandBuffer cmd);

        /**
         * @brief Called only after the frame's fence has been signaled.
         */
        void collect(uint32_t frameIndex, VkDevice device);

        float getRegionTimeMs(const std::string& name) const;

        std::vector<std::pair<std::string, float>> getAllRegionTimes() const;

        /**
         * @brief GPU time between being and end frame
         */

        /**
         * @brief GPU time between begin and end frame
         */
        float getTotalFrameTimeMs() const;

        TracyVkCtx& getTracyCtx();

    private:
        struct RegionQuery {
            uint32_t startQueryIndex;
            uint32_t endQueryIndex;
        };

        struct FrameQueries {
            uint32_t startQueryIndex;
            uint32_t endQueryIndex;

            bool hasResults = false;

            std::unordered_map<std::string, RegionQuery> regions;
        };

        uint32_t frameQueryBase(uint32_t frameIndex) const;

        VkQueryPool queryPool_ = VK_NULL_HANDLE;

        float timestampPeriod_ = 0.0f;

        uint32_t maxRegionsPerFrame_ = 32;
        uint32_t framesInFlight_ = 0;

        // Two queries for frame timing
        uint32_t queriesPerFrame_ = 0;
        uint32_t queryCount_ = 0;

        uint32_t currentFrameIndex_ = 0;
        uint32_t nextFreeQueryIndex_ = 0;

        std::vector<FrameQueries> frames_;

        // Results from the most recently collected frame
        std::unordered_map<std::string, float> lastResultsMs_;
        float lastFrameTimeMs_ = 0.0f;

        TracyVkCtx profilerCtx_ = nullptr;
    };

    /**
     * @brief Scoped gpu region for tracy
     */
    class ScopedGpuRegion
    {
    public:
        ScopedGpuRegion(GpuProfiler& profiler, VkCommandBuffer cmd, std::string name);
        ~ScopedGpuRegion();

        ScopedGpuRegion(const ScopedGpuRegion&) = delete;
        ScopedGpuRegion& operator=(const ScopedGpuRegion&) = delete;

    private:
        GpuProfiler& profiler_;
        VkCommandBuffer cmd_;
        std::string name_;
    };
} // namespace chai::gfx

#define CHAI_GPU_ZONE(profiler, tracyCtx, cmd, name)                                        \
    TracyVkZone(tracyCtx, cmd, name);                                                       \
    ::chai::gfx::ScopedGpuRegion chaiGpuRegion_##__LINE__(profiler, cmd, name)

#define CHAI_GPU_ZONE_DYNAMIC(profiler, tracyCtx, cmd, namevar)                             \
    TracyVkZoneTransient(tracyCtx, chaiTracyZone_##__LINE__, cmd, (namevar).c_str(), true); \
    ::chai::gfx::ScopedGpuRegion chaiGpuRegion_##__LINE__(profiler, cmd, namevar)