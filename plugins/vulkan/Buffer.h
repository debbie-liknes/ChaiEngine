#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <Log.h>

namespace chai::gfx
{
    struct Buffer {
        VkBuffer handle = VK_NULL_HANDLE;
        VmaAllocation allocation = nullptr;
        VmaAllocationInfo info = {}; // pMappedData (if mapped), size, memory type

        bool valid() const { return handle != VK_NULL_HANDLE; }
        void* mapped() const { return info.pMappedData; }
    };

    inline Buffer createBuffer(VmaAllocator allocator,
                        VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VmaAllocationCreateFlags allocFlags)
    {
        VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO; // intent-based; VMA picks the type
        allocInfo.flags = allocFlags;

        Buffer buffer;
        if (vmaCreateBuffer(allocator,
                            &bufferInfo,
                            &allocInfo,
                            &buffer.handle,
                            &buffer.allocation,
                            &buffer.info) != VK_SUCCESS) {
            CHAI_LOG_ERROR("createBuffer failed (size={})", size);
            return {}; // handle stays VK_NULL_HANDLE -> valid() == false
        }
        return buffer;
    }

    // Frees right now. Only safe when no in-flight frame can reference it —
    // i.e. staging buffers (we fence-waited) and error-path cleanup.
    inline void destroyBufferImmediate(VmaAllocator allocator, Buffer& b)
    {
        if (b.valid())
            vmaDestroyBuffer(allocator, b.handle, b.allocation);
        b = {};
    }
}