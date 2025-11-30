#pragma once
#include <Asset/AssetLoader.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <TextureLoaderExport.h>


namespace chai
{
    class TEXTURELOADER_EXPORT PngLoader : public IAssetLoader
    {
    public:
        PngLoader() = default;

        bool canLoad(const std::string& ext) const override;
        std::unique_ptr<IAsset> load(const std::string& path) override;
    };
}

CHAI_PLUGIN_CLASS(ImageLoaderPlugin)
CHAI_DECLARE_PLUGIN_ENTRY(TEXTURELOADER_EXPORT)