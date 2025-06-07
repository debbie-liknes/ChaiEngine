#pragma once
#include <Resource/ResourceLoader.h>
#include <Types/Texture2D.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>


namespace chai
{
    class PngLoader : public IResourceLoader {
    public:
        PngLoader();

        bool CanLoad(const std::string& ext) const override;
        std::shared_ptr<IResource> Load(const std::string& path) override;
    };
}

CHAI_PLUGIN_CLASS(ImageLoaderPlugin) {
    CHAI_SERVICE(chai::PngLoader, "PNG loader");
}
CHAI_REGISTER_PLUGIN(ImageLoaderPlugin, "TextureLoader", "1.0.0")