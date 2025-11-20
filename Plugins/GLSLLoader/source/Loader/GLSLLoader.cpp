#include <Loader/GLSLLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace chai
{
    bool GLSLLoader::canLoad(const std::string& ext) const
    {
        return ext == ".vert" || ext == ".frag";
    }

    std::unique_ptr<IAsset> GLSLLoader::load(const std::string& path)
    {
        return nullptr;
    }
}