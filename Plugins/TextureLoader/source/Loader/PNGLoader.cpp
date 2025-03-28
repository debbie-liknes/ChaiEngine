#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

namespace chai
{
    bool PngLoader::CanLoad(const std::string& ext) const
    {
        return ext == ".png";
    }

    std::shared_ptr<IResource> PngLoader::Load(const std::string& path)
    {
        int w, h, channels;
        stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
        if (!pixels) return nullptr;

        auto tex = std::make_shared<Texture2D>();
        tex->create(w, h, pixels);
        stbi_image_free(pixels);
        return tex;
    }
}