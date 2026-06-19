/**
 * @file DefaultTextures.h
 */
#pragma once
#include <Assets/TextureAsset.h>

namespace chai::gfx
{
    /**
     * @brief Creates a 1x1 white pixel
     */
    TextureAsset createWhiteTexture();

    /**
     * @brief Creates a 1x1 normal pixel
     */
    TextureAsset createDefaultNormalTexture();
}