#include <Assets/DefaultTextures.h>

namespace chai::gfx
{
    TextureAsset createWhiteTexture()
    {
        TextureAsset texture;
        texture.width = 1;
        texture.height = 1;
        texture.channels = 4;
        texture.bytesPerChannel = 1;
        texture.pixels = {255, 255, 255, 255};
        texture.format = TextureFormat::RGBA8_SRGB;

        return texture;
    }

    TextureAsset createDefaultNormalTexture()
    {
        TextureAsset texture;
        texture.width = 1;
        texture.height = 1;
        texture.channels = 4;
        texture.bytesPerChannel = 1;
        texture.pixels = {128, 128, 255, 255};
        texture.format = TextureFormat::RGBA8_UNORM;

        return texture;
    }
}