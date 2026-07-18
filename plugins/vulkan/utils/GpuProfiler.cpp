#include "GpuProfiler.h"

#include "VkCheck.h"

namespace chai::gfx
{
    void GpuProfiler::initialize(VkDevice device,
                                 VkPhysicalDevice physicalDevice,
                                 uint32_t framesInFlight)
    {
        framesInFlight_ = framesInFlight;
        maxRegionsPerFrame_ = 32;
        queryCount_ = framesInFlight * maxRegionsPerFrame_ * 2;

        VkQueryPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        info.queryType = VK_QUERY_TYPE_TIMESTAMP;
        info.queryCount = queryCount_;
        VK_CHECK(vkCreateQueryPool(device, &info, nullptr, &queryPool_));

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevice, &props);
        timestampPeriod_ = props.limits.timestampPeriod;

        regionsPerFrame_.resize(framesInFlight);
    }

    void GpuProfiler::shutdown(VkDevice device)
    {
        vkDestroyQueryPool(device, queryPool_, nullptr);
    }

    void GpuProfiler::beginFrame(VkCommandBuffer cmd, uint32_t frameIndex)
    {
        uint32_t base = frameIndex * maxRegionsPerFrame_ * 2;
        vkCmdResetQueryPool(cmd, queryPool_, 0, queryCount_);

        regionsPerFrame_[frameIndex].clear();
        nextFreeQueryIndex_ = 0;
        currentFrameIndex_ = frameIndex;
    }

    void GpuProfiler::beginRegion(VkCommandBuffer cmd, const std::string& name)
    {
        uint32_t frameIndex = currentFrameIndex_;
        uint32_t base = frameIndex * maxRegionsPerFrame_ * 2;
        uint32_t startIdx = base + nextFreeQueryIndex_++;
        uint32_t endIdx = base + nextFreeQueryIndex_++;

        regionsPerFrame_[frameIndex][name] = {startIdx, endIdx};
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool_, startIdx);
    }

    void GpuProfiler::endRegion(VkCommandBuffer cmd, const std::string& name)
    {
        uint32_t frameIndex = currentFrameIndex_;
        uint32_t endIdx = regionsPerFrame_[frameIndex][name].endQueryIndex;
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool_, endIdx);
    }

    void GpuProfiler::endFrame(uint32_t frameIndex, VkDevice device)
    {
        for (auto& [name, region] : regionsPerFrame_[frameIndex]) {
            uint64_t timestamps[2];
            VkResult result = vkGetQueryPoolResults(device,
                                                    queryPool_,
                                                    region.startQueryIndex,
                                                    2,
                                                    sizeof(timestamps),
                                                    timestamps,
                                                    sizeof(uint64_t),
                                                    VK_QUERY_RESULT_64_BIT);

            if (result == VK_SUCCESS) {
                lastResultsMs_[name] =
                    static_cast<float>((timestamps[1] - timestamps[0]) * timestampPeriod_ * 1e-6);
            }
        }
    }

    float GpuProfiler::getRegionTimeMs(const std::string& name) const
    {
        auto it = lastResultsMs_.find(name);
        return it != lastResultsMs_.end() ? it->second : 0.0f;
    }

    std::vector<std::pair<std::string, float>> GpuProfiler::getAllRegionTimes() const
    {
        return {lastResultsMs_.begin(), lastResultsMs_.end()};
    }

    float GpuProfiler::getTotalFrameTimeMs() const
    {
        float total = 0.0f;
        for (const auto& [name, time] : lastResultsMs_) {
            total += time;
        }
        return total;
    }

} // namespace chai::gfx