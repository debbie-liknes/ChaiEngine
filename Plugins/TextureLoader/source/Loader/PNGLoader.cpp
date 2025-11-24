#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include "ChaiEngine/TextureAsset.h"

#include <stb_image.h>

namespace chai
{
    bool PngLoader::canLoad(const std::string& ext) const
    {
        return ext == "png";
    }

    std::unique_ptr<IAsset> PngLoader::load(const std::string& path)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error(std::string("Failed to load image: ") + stbi_failure_reason());
        }

        return std::make_unique<TextureAsset>(width, height, channels, pixels);
    }
}