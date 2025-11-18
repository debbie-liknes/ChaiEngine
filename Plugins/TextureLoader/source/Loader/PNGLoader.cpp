#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace chai
{
    bool PngLoader::canLoad(const std::string& ext) const
    {
        return ext == ".png";
    }

    std::unique_ptr<IAsset> PngLoader::load(const std::string& path)
    {
        return nullptr;
    }
}