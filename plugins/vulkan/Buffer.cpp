#include "Buffer.h"

namespace chai::gfx
{
    Buffer createBuffer(VmaAllocator allocator,
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

    void destroyBufferImmediate(VmaAllocator allocator, Buffer& b)
    {
        if (b.valid())
            vmaDestroyBuffer(allocator, b.handle, b.allocation);
        b = {};
    }
}