#pragma once
#include <functional>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace chai::gfx
{
	class UploadContext
	{
    public:
        UploadContext() = default;
        ~UploadContext();

        UploadContext(const UploadContext&) = delete;
        UploadContext& operator=(const UploadContext&) = delete;

        void init(VkDevice device, VkQueue queue, uint32_t queueFamilyIndex);
        void shutdown();

        uint64_t submit(std::function<void(VkCommandBuffer)> record);

        uint64_t completedValue() const;
        uint64_t lastSubmittedValue() const { return nextUploadId_; }
        void waitFor(uint64_t value) const;


	private:
        //we own these
        uint64_t nextUploadId_ = 0; //assums single thread
        VkCommandPool asyncPool_ = VK_NULL_HANDLE;
        VkSemaphore timeline_ = VK_NULL_HANDLE;

        //we do NOT own these
        VkQueue transferQueue_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;

        struct InFlight {
            VkCommandBuffer cmd;
            uint64_t value;
        };
        std::vector<InFlight> inFlight_;
        std::vector<VkCommandBuffer> free_;

        InFlight& allocFromUploadPool();
	};
}