#include "UploadContext.h"
#include "../utils/VkCheck.h"

namespace chai::gfx
{
    UploadContext::~UploadContext() {}

    uint64_t UploadContext::submit(std::function<void(VkCommandBuffer)> record)
    {
        InFlight& inFlight = allocFromUploadPool();

        VK_CHECK(vkResetCommandBuffer(inFlight.cmd, 0));

        VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(inFlight.cmd, &begin));
        record(inFlight.cmd); // caller's commands
        VK_CHECK(vkEndCommandBuffer(inFlight.cmd));

        VkCommandBufferSubmitInfo cmdInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
        cmdInfo.commandBuffer = inFlight.cmd;

        VkSemaphoreSubmitInfo sig{VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
        sig.semaphore = timeline_;
        sig.value = inFlight.value;
        sig.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

        VkSubmitInfo2 submit{VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
        submit.commandBufferInfoCount = 1;
        submit.pCommandBufferInfos = &cmdInfo;
        submit.signalSemaphoreInfoCount = 1;
        submit.pSignalSemaphoreInfos = &sig;

        VK_CHECK(vkQueueSubmit2(transferQueue_, 1, &submit, VK_NULL_HANDLE));

        return inFlight.value;
    }

    UploadContext::InFlight& UploadContext::allocFromUploadPool()
    {
        VkCommandBuffer buff = VK_NULL_HANDLE;
        if (!free_.empty()) {
            buff = free_.back();
            free_.pop_back();
        } else {
            VkCommandBufferAllocateInfo cmdInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
            cmdInfo.commandPool = asyncPool_;
            cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdInfo.commandBufferCount = 1;
            vkAllocateCommandBuffers(device_, &cmdInfo, &buff);
        }

        return inFlight_.emplace_back(InFlight{buff, ++nextUploadId_});
    }

    void UploadContext::init(VkDevice device, VkQueue queue, uint32_t queueFamilyIndex)
    {
        device_ = device;
        transferQueue_ = queue;

        VkCommandPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        poolInfo.flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndex;
        vkCreateCommandPool(device_, &poolInfo, nullptr, &asyncPool_);

        VkSemaphoreTypeCreateInfo type{VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
        type.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        type.initialValue = 0;

        VkSemaphoreCreateInfo semCreate{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        semCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semCreate.pNext = &type;
        semCreate.flags = 0;

        VK_CHECK(vkCreateSemaphore(device, &semCreate, nullptr, &timeline_));
    }

    void UploadContext::shutdown()
    {
        vkDestroyCommandPool(device_, asyncPool_, nullptr);
        vkDestroySemaphore(device_, timeline_, nullptr);
    }

    uint64_t UploadContext::completedValue() const
    {
        uint64_t v = 0;
        vkGetSemaphoreCounterValue(device_, timeline_, &v);
        return v;
    }

    void UploadContext::waitFor(uint64_t value) const
    {
        VkSemaphoreWaitInfo wait{VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
        wait.semaphoreCount = 1;
        wait.pSemaphores = &timeline_;
        wait.pValues = &value;
        vkWaitSemaphores(
            device_, &wait, UINT64_MAX);
    }
}