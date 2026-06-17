#pragma once
#include <Loaders/IModelLoader.h>
#include <Assets/ModelAsset.h>
#include <filesystem>

namespace chai::gfx
{

    class GLTFLoader : public IModelLoader
    {
    public:
        GLTFLoader() = default;

        bool canLoad(std::string_view ext) const override;
        const char* name() const override;
        std::optional<ModelAsset> decode(std::span<const uint8_t> bytes,
                                                 const std::filesystem::path& baseDir) override;
    };
} // namespace chai