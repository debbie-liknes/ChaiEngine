#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace chai
{
    bool PngLoader::canLoad(const std::string& ext) const
    {
        return ext == ".png";
    }

    std::shared_ptr<IResource> PngLoader::load(const std::string& path)
    {
        return nullptr;
    }
}