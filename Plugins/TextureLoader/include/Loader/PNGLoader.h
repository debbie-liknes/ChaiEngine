#pragma once
#include <Asset/AssetLoader.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>


namespace chai
{
    class PngLoader : public IAssetLoader
    {
    public:
        PngLoader() = default;

        bool canLoad(const std::string& ext) const override;
        std::unique_ptr<IAsset> load(const std::string& path) override;
    };
}

CHAI_PLUGIN_CLASS (ImageLoaderPlugin)
{
    CHAI_LOADER(chai::PngLoader, "PNG loader")
}
CHAI_REGISTER_PLUGIN(ImageLoaderPlugin, "TextureLoader", "1.0.0")