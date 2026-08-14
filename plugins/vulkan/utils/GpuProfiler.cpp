#include "GpuProfiler.h"

#include "VkCheck.h"

namespace chai::gfx
{
    void GpuProfiler::initialize(VkDevice device,
                                 VkPhysicalDevice physicalDevice,
                                 VkQueue queue,
                                 VkCommandBuffer setupBuff,
                                 uint32_t framesInFlight)
    {
        framesInFlight_ = framesInFlight;

        queriesPerFrame_ = 2 + maxRegionsPerFrame_ * 2;
        queryCount_ = framesInFlight_ * queriesPerFrame_;

        VkQueryPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        info.queryType = VK_QUERY_TYPE_TIMESTAMP;
        info.queryCount = queryCount_;

        VK_CHECK(vkCreateQueryPool(device, &info, nullptr, &queryPool_));

        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(physicalDevice, &props);

        timestampPeriod_ = props.limits.timestampPeriod;

        frames_.resize(framesInFlight_);

        profilerCtx_ = TracyVkContext(physicalDevice, device, queue, setupBuff);
    }

    void GpuProfiler::shutdown(VkDevice device)
    {
        if (queryPool_ != VK_NULL_HANDLE) {
            vkDestroyQueryPool(device, queryPool_, nullptr);

            queryPool_ = VK_NULL_HANDLE;
        }

        if (profilerCtx_ != nullptr) {
            TracyVkDestroy(profilerCtx_);
            profilerCtx_ = nullptr;
        }
    }

    uint32_t GpuProfiler::frameQueryBase(uint32_t frameIndex) const
    {
        return frameIndex * queriesPerFrame_;
    }

    void GpuProfiler::beginFrame(VkCommandBuffer cmd, uint32_t frameIndex)
    {
        currentFrameIndex_ = frameIndex;
        nextFreeQueryIndex_ = 0;

        FrameQueries& frame = frames_[frameIndex];

        frame.regions.clear();

        const uint32_t base = frameQueryBase(frameIndex);

        frame.startQueryIndex = base;
        frame.endQueryIndex = base + 1;

        // Only reset this frame slot queries.
        vkCmdResetQueryPool(cmd, queryPool_, base, queriesPerFrame_);

        // start the timestamp
        vkCmdWriteTimestamp(
            cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool_, frame.startQueryIndex);
    }

    void GpuProfiler::beginRegion(VkCommandBuffer cmd, const std::string& name)
    {
        FrameQueries& frame = frames_[currentFrameIndex_];

        if (nextFreeQueryIndex_ >= maxRegionsPerFrame_) {
            return;
        }

        const uint32_t base = frameQueryBase(currentFrameIndex_);

        const uint32_t startQuery = base + 2 + nextFreeQueryIndex_ * 2;

        const uint32_t endQuery = startQuery + 1;

        ++nextFreeQueryIndex_;

        frame.regions[name] = {startQuery, endQuery};

        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool_, startQuery);
    }

    void GpuProfiler::endRegion(VkCommandBuffer cmd, const std::string& name)
    {
        FrameQueries& frame = frames_[currentFrameIndex_];

        auto it = frame.regions.find(name);

        if (it == frame.regions.end()) {
            return;
        }

        vkCmdWriteTimestamp(
            cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool_, it->second.endQueryIndex);
    }

    void GpuProfiler::endFrame(VkCommandBuffer cmd)
    {
        FrameQueries& frame = frames_[currentFrameIndex_];

        // Timestamp end
        vkCmdWriteTimestamp(
            cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool_, frame.endQueryIndex);

        frame.hasResults = true;
    }

    void GpuProfiler::collect(uint32_t frameIndex, VkDevice device)
    {
        const FrameQueries& frame = frames_[frameIndex];

        //saves the first fence wait from causing a validation error
        if (!frame.hasResults)
            return;

        // Queries should be available if it was called after the fence sync

        uint64_t frameTimestamps[2]{};

        VkResult result = vkGetQueryPoolResults(device,
                                                queryPool_,
                                                frame.startQueryIndex,
                                                2,
                                                sizeof(frameTimestamps),
                                                frameTimestamps,
                                                sizeof(uint64_t),
                                                VK_QUERY_RESULT_64_BIT);

        if (result == VK_SUCCESS) {
            lastFrameTimeMs_ = static_cast<float>((frameTimestamps[1] - frameTimestamps[0]) *
                                                  timestampPeriod_ * 1e-6);
        }

        lastResultsMs_.clear();

        for (const auto& [name, region] : frame.regions) {
            uint64_t timestamps[2]{};

            result = vkGetQueryPoolResults(device,
                                           queryPool_,
                                           region.startQueryIndex,
                                           2,
                                           sizeof(timestamps),
                                           timestamps,
                                           sizeof(uint64_t),
                                           VK_QUERY_RESULT_64_BIT);

            if (result != VK_SUCCESS) {
                continue;
            }

            lastResultsMs_[name] =
                static_cast<float>((timestamps[1] - timestamps[0]) * timestampPeriod_ * 1e-6);
        }
    }

    float GpuProfiler::getRegionTimeMs(const std::string& name) const
    {
        auto it = lastResultsMs_.find(name);

        if (it == lastResultsMs_.end()) {
            return 0.0f;
        }

        return it->second;
    }

    std::vector<std::pair<std::string, float>> GpuProfiler::getAllRegionTimes() const
    {
        return {lastResultsMs_.begin(), lastResultsMs_.end()};
    }

    float GpuProfiler::getTotalFrameTimeMs() const
    {
        return lastFrameTimeMs_;
    }
} // namespace chai::gfx