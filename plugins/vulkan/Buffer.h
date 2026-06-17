/**
 * @file Buffer.h
 */
#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <Log.h>

namespace chai::gfx
{
    /**
     * @brief A chunk of memory for a gpu resource (ex. vertex buffers)
     */
    struct Buffer 
    {
        VkBuffer handle = VK_NULL_HANDLE;
        VmaAllocation allocation = nullptr;
        VmaAllocationInfo info = {};

        bool valid() const { return handle != VK_NULL_HANDLE; }
        void* mapped() const { return info.pMappedData; }
    };

    /**
     * @brief Helper function to make a buffer given an allocator
     */
    Buffer createBuffer(VmaAllocator allocator,
                               VkDeviceSize size,
                               VkBufferUsageFlags usage,
                               VmaAllocationCreateFlags allocFlags);

     /**
     * @brief Dont defer, free the buffer now
     */
    void destroyBufferImmediate(VmaAllocator allocator, Buffer& b);
}