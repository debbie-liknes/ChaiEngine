#include "GLTFLoader.h"
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>


namespace chai::gfx
{
    bool GLTFLoader::canLoad(std::string_view ext) const
    {
        return ext == "gltf";
    }

    const char* GLTFLoader::name() const
    {
        return "GLTFLoader";
    }

    std::optional<ModelAsset> GLTFLoader::decode(std::span<const uint8_t> bytes,
        const std::filesystem::path& baseDir)
    {
        ModelAsset a;
        return a;
    }
} // namespace chai