#pragma once
#include <Resource/ResourceLoader.h>
#include <Types/Texture2D.h>
#include <Meta/ChaiMacros.h>
#include <Plugin/PluginRegistry.h>


namespace chai
{
    class PngLoader : public IResourceLoader {
    public:
        bool CanLoad(const std::string& ext) const override;
        std::shared_ptr<IResource> Load(const std::string& path) override;
    };

    CHAI_CLASS(chai::PngLoader)
    END_CHAI()
}

//this could probably live in a different file
void registerServices()
{
    chai::kettle::PluginRegistry::Instance().Register("Loader", "TextureLoader", [] {
        return static_cast<void*>(new chai::PngLoader());
        });
}

CHAI_PLUGIN(TextureLoader, "1.0.0", "chai", "Loader", registerServices)