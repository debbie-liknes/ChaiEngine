#pragma once
#include <Resource/ResourceLoader.h>
#include <Types/Texture2D.h>
#include <Meta/ChaiMacros.h>
#include <Plugin/PluginRegistry.h>
#include <ChaiEngine/Renderer.h>
#include <Plugin/ServiceLocator.h>


namespace chai
{
    class PngLoader : public IResourceLoader {
    public:
        PngLoader();
        PngLoader(brew::Renderer* renderer) : m_renderer(renderer) {}

        bool CanLoad(const std::string& ext) const override;
        std::shared_ptr<IResource> Load(const std::string& path) override;

    private:
        brew::Renderer* m_renderer;
    };

    CHAI_CLASS(chai::PngLoader)
    END_CHAI()
}

//this could probably live in a different file
void registerServices()
{
    chai::kettle::PluginRegistry::Instance().Register("Loader", "TextureLoader", [] {
        auto renderer = chai::ServiceLocator::Get<chai::brew::Renderer>();
        return static_cast<void*>(new chai::PngLoader(renderer.get()));
        });
}

CHAI_PLUGIN(TextureLoader, "1.0.0", "chai", "Loader", registerServices)