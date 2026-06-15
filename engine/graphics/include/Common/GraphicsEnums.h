#pragma once
#include <cstdint>
#include <Common/GraphicsMacros.h>

namespace chai::gfx
{
    enum class ShaderStage : std::uint32_t 
    {
        None = 0,
        Vertex = 1u << 0,
        Fragment = 1u << 1,
        Compute = 1u << 2,
        Geometry = 1u << 3,
        TessControl = 1u << 4,
        TessEval = 1u << 5,
        AllGraphics = (1u << 0) | (1u << 1) | (1u << 3) | (1u << 4) | (1u << 5),
    };
    CHAI_FLAG_OPS(ShaderStage)

    enum class BufferUsage : std::uint32_t 
    {
        None = 0,
        Vertex = 1u << 0,
        Index = 1u << 1,
        Uniform = 1u << 2,
        Storage = 1u << 3,
        Indirect = 1u << 4,
        TransferSrc = 1u << 5,
        TransferDst = 1u << 6,
    };
    CHAI_FLAG_OPS(BufferUsage)

    enum class BindingType : std::uint8_t
    {
        UniformBuffer
    };
    CHAI_FLAG_OPS(BindingType)

    enum class AlphaMode
    {
        Opaque,
        Mask,
        Blend
    };
    CHAI_FLAG_OPS(AlphaMode)
}