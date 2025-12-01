#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/TextureAsset.h"

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
        TextureFace face;
        face.width = width;
        face.height = height;
        face.channels = channels;
        face.pixels.resize(width * height * channels);
        memcpy(face.pixels.data(), pixels, width * height * channels);

        stbi_image_free(pixels);

        return std::make_unique<TextureAsset>(std::vector<TextureFace>{face});
    }
}

CHAI_PLUGIN_SERVICES(ImageLoaderPlugin)
{
    CHAI_LOADER(chai::PngLoader, "PNG loader")
}
CHAI_DEFINE_PLUGIN_ENTRY(ImageLoaderPlugin, "TextureLoader", "1.0.0", TEXTURELOADER_EXPORT)