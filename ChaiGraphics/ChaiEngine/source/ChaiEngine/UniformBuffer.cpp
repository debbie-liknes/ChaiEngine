#include <ChaiEngine/UniformBuffer.h>

namespace chai
{
    UniformBufferBase::~UniformBufferBase()
    {
    }

    uint64_t UniformBufferBase::getNextId()
    {
        static uint64_t currentId = 0;
        return ++currentId;
    }
}