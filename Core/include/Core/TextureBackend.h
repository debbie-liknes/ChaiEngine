#pragma once
#include <cstdint>

namespace chai
{
    class ITextureBackend {
    public:
        virtual ~ITextureBackend() = default;
        virtual void Bind(uint32_t slot) = 0;
        virtual void* GetNativeHandle() = 0;
    };
}