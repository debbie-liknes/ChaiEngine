#pragma once
#include <ModelLoaderExport.h>
#include <Meta/ChaiMacros.h>
#include <Asset/AssetLoader.h>
#include "GLTFLoader.h"

namespace chai
{
    class MODELLOADER_EXPORT ObjLoader : public IAssetLoader
    {
    public:
        ObjLoader() = default;

        bool canLoad(const std::string& ext) const override;
        std::unique_ptr<IAsset> load(const std::string& path) override;
    };
}

CHAI_PLUGIN_CLASS(ModelLoader)
CHAI_DECLARE_PLUGIN_ENTRY(MODELLOADER_EXPORT)