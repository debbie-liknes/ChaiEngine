/**
 * @file TextureAsset.h
 */
#pragma once
#include <Assets/TextureFormat.h>

namespace chai::gfx
{
    struct Texture;

    /**
     * @brief CPU side data for a Texture
     */
    struct TextureAsset 
    {
        uint32_t width = 0, height = 0;
        uint32_t channels = 4;
        bool isCube = false;
        uint32_t layerCount = 1;
        std::vector<uint8_t> pixels; //data will be concatenated if cube texture
        uint8_t bytesPerChannel = 1;
        TextureFormat format = TextureFormat::RGBA8_SRGB;

        bool isValid() const { return !pixels.empty() && width > 0 && height > 0; }
    };
} // namespace chai