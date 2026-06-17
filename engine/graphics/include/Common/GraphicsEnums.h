/**
 * @file GraphicsEnums.h
 */
#pragma once
#include <cstdint>
#include <Common/GraphicsMacros.h>

namespace chai::gfx
{
    /**
     * @brief Used by materials, tells the pipeline to use blending or not
     */
    enum class AlphaMode
    {
        Opaque,
        Mask,
        Blend
    };
    CHAI_FLAG_OPS(AlphaMode)
}