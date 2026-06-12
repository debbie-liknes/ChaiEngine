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

        return texture;
    }
}