#pragma once
#include <GLSLLoaderExport.h>
#include <Asset/AssetLoader.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>


namespace chai
{
    class GLSLLOADER_EXPORT GLSLLoader : public IAssetLoader
    {
    public:
        GLSLLoader() = default;
        virtual ~GLSLLoader() = default;

        bool canLoad(const std::string& ext) const override;
        std::unique_ptr<IAsset> load(const std::string& path) override;
    };
}

CHAI_PLUGIN_CLASS(ShaderLoaderPlugin)
CHAI_DECLARE_PLUGIN_ENTRY(GLSLLOADER_EXPORT)