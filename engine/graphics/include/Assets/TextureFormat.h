#pragma once
#include <AssetCache.h>
#include <AssetCommon.h>
#include <Handle.h>

namespace chai::gfx
{
    enum class TextureFormat 
    {
        // sRGB
        RGBA8_SRGB,

        // color
        RGBA8_UNORM
    };
} // namespace chai