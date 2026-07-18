#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace chai::gfx
{
    class GpuProfiler
    {
    public:
        void initialize(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t framesInFlight);
        void shutdown(VkDevice device);

        void beginFrame(VkCommandBuffer cmd, uint32_t frameIndex);

        void beginRegion(VkCommandBuffer cmd, const std::string& name);
        void endRegion(VkCommandBuffer cmd, const std::string& name);

        void endFrame(uint32_t frameIndex, VkDevice device);

        float getRegionTimeMs(const std::string& name) const;
        std::vector<std::pair<std::string, float>> getAllRegionTimes() const;
        float getTotalFrameTimeMs() const;

    private:
        struct RegionQuery {
            uint32_t startQueryIndex;
            uint32_t endQueryIndex;
        };

        VkQueryPool queryPool_;
        float timestampPeriod_;
        uint32_t maxRegionsPerFrame_;
        uint32_t framesInFlight_;
        uint32_t queryCount_ = 64;
        uint32_t currentFrameIndex_ = 0;

        std::vector<std::unordered_map<std::string, RegionQuery>> regionsPerFrame_;
        std::unordered_map<std::string, float> lastResultsMs_;
        uint32_t nextFreeQueryIndex_ = 0;
    };
} // namespace gfx