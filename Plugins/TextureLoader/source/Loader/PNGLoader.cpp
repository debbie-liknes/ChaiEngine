#include <Loader/PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace chai
{
    PngLoader::PngLoader()
    {

    }

    bool PngLoader::canLoad(const std::string& ext) const
    {
        return ext == ".png";
    }

    std::shared_ptr<IResource> PngLoader::load(const std::string& path)
    {
        //if (!m_renderer) return nullptr;

        //int w, h, channels;
        //stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
        //if (!pixels) return nullptr;

        //auto backend = m_renderer->createTexture2D(pixels, w, h);
        //auto tex = std::make_shared<Texture2D>(Texture2D{ backend, (uint32_t)w, (uint32_t)h});
        //stbi_image_free(pixels);
        //return tex;
        return nullptr;
    }
}