/**
 * @file TextureFormat.h
 */
#pragma once
#include <AssetCache.h>
#include <AssetCommon.h>
#include <Handle.h>

namespace chai::gfx
{
    /**
     * @todo add all texture formats, theses are just the ones I use
     */
    enum class TextureFormat 
    {
        // sRGB
        RGBA8_SRGB,

        // color
        RGBA8_UNORM
    };
} // namespace chai