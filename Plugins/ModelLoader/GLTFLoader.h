#pragma once
#include <Meta/ChaiMacros.h>
#include <Asset/AssetLoader.h>

namespace chai
{

    class GLTFLoader : public IAssetLoader
    {
    public:
        GLTFLoader() = default;

        bool canLoad(const std::string& ext) const override;
        std::unique_ptr<IAsset> load(const std::string& path) override;
    };
} // namespace chai