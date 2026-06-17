#pragma once
#include <Assets/ITextureRegistry.h>
#include <Plugin/ServiceLocator.h>
#include <Log.h>
#include <Loaders/ITextureLoader.h>
#include <Core/FileIO.h>
#include <AssetCache.h>
#include <Assets/TextureAsset.h>
#include <memory>
#include "TextureFactory.h"

namespace chai::gfx
{
    // concrete impl
    class TextureRegistry : public ITextureRegistry
    {
    public:
        TextureRegistry(ResourceFactory<Texture>& factory,
                        DeferredDeleteQueue& graveyard,
                        ServiceLocator* locator);

        ~TextureRegistry() override;

        std::shared_ptr<AssetCache<Texture>> cache();

        Handle<Texture> ingest(AssetId id, TextureAsset asset) override;
        Handle<Texture> loadCubemap(AssetId id,
                                    const std::array<std::filesystem::path, 6>& files) override;
        Handle<Texture> load(AssetId id, const std::filesystem::path& path) override;
        void release(Handle<Texture> h) override;

    private:
        std::shared_ptr<AssetCache<Texture>> cache_;
        ServiceLocator* locator_;
    };
}