#include "TextureRegistry.h"

namespace chai::gfx
{
    TextureRegistry::TextureRegistry(ResourceFactory<Texture>& factory,
                    DeferredDeleteQueue& graveyard,
                    ServiceLocator* locator)
        : locator_(locator)
    {
        cache_ = std::make_shared<AssetCache<Texture>>(factory, graveyard);
    }

    TextureRegistry::~TextureRegistry() 
    {
        locator_ = nullptr;
    }

    std::shared_ptr<AssetCache<Texture>> TextureRegistry::cache()
    {
        return cache_;
    }

    Handle<Texture> TextureRegistry::ingest(AssetId id, TextureAsset asset) 
    {
        if (!asset.isValid()) {
            CHAI_LOG_ERROR("ingest: invalid TextureAsset for '{}'", id.value);
            return {};
        }
        return cache_->ingest(id, std::move(asset));
    }

    Handle<Texture>
    TextureRegistry::loadCubemap(AssetId id,
                                const std::array<std::filesystem::path, 6>& files) 
    {
        auto loader = locator_->tryResolve<ITextureLoader>();
        if (!loader) {
            CHAI_LOG_ERROR("No image loader for cubemap");
            return {};
        }

        TextureAsset cube{};
        cube.isCube = true;
        cube.layerCount = 6;

        for (size_t i = 0; i < 6; ++i) {
            auto bytes = readFileBytes(files[i]);
            if (bytes.empty()) {
                CHAI_LOG_ERROR("Could not read cubemap face {}", files[i].string());
                return {};
            }
            auto image = loader->decode(bytes, TextureFormat::RGBA8_SRGB);
            if (!image)
                return {};

            if (i == 0) {
                cube.width = image->width;
                cube.height = image->height;
                cube.channels = image->channels;
                cube.bytesPerChannel = image->bytesPerChannel;
                cube.format = image->format;
                cube.pixels.reserve(image->pixels.size() * 6);
            } else if (image->width != cube.width || image->height != cube.height) {
                CHAI_LOG_ERROR("cubemap face {} size mismatch ({}x{} vs {}x{})",
                               i,
                               image->width,
                               image->height,
                               cube.width,
                               cube.height);
                return {};
            }
            // append this faces bytes
            cube.pixels.insert(cube.pixels.end(), image->pixels.begin(), image->pixels.end());
        }
        return ingest(id, std::move(cube));
    }

    Handle<Texture> TextureRegistry::load(AssetId id, const std::filesystem::path& path) 
    {
        if (Handle<Texture> tex = cache_->acquire(id); tex.valid())
            return tex;

        auto bytes = readFileBytes(path);
        if (bytes.empty()) {
            return {};
        }

        auto ext = path.extension().string();
        if (!ext.empty() && ext.front() == '.')
            ext.erase(0, 1);

        auto loader = locator_->tryResolve<ITextureLoader>();
        if (!loader) {
            CHAI_LOG_ERROR("No image loader for '.{}' ('{}')", ext, path.string());
            return {};
        }

        auto image = loader->decode(bytes, TextureFormat::RGBA8_SRGB);
        if (!image)
            return {};

        return ingest(id, std::move(*image));
    }

    void TextureRegistry::release(Handle<Texture> h) 
    {
        cache_->release(h);
    }

    Handle<Texture> TextureRegistry::get(AssetId id)
    {
        return cache_->acquire(id);
    }

    Handle<Texture> TextureRegistry::reserve(AssetId id)
    {
        return cache_->acquire(id);
    }
}