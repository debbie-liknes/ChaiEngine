#pragma once
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include <Handle.h>
#include <AssetCommon.h>
#include <AssetCache.h>
#include <filesystem>

namespace chai
{
    namespace gfx
    {
        class TextureFactory;
    }
    struct Texture;

    enum class TextureFormat 
    { 
        //sRGB
        RGBA8_SRGB, 

        //color
        RGBA8_UNORM 
    };

    //cpu data
    struct TextureAsset
    {
        uint32_t width = 0, height = 0;
        uint32_t channels = 4;
        std::vector<uint8_t> pixels;
        uint8_t bytesPerChannel = 1;
        TextureFormat format = TextureFormat::RGBA8_SRGB;

        bool isValid() const { return !pixels.empty() && width > 0 && height > 0; }
    };

    //bytes -> image asset
    class ITextureLoader
    {
    public:
        virtual ~ITextureLoader() = default;
        virtual bool canLoad(std::string_view ext) const = 0;
        virtual std::optional<TextureAsset> decode(std::span<const uint8_t> bytes) const = 0;
        virtual const char* name() const = 0;
    };

    //bookkeper
    class ITextureRegistry
    {
    public:
        virtual ~ITextureRegistry() = default;

        virtual Handle<Texture> ingest(AssetId, TextureAsset) = 0;
        virtual Handle<Texture> load(AssetId, const std::filesystem::path&) = 0;
        virtual void release(Handle<Texture>) = 0;
    };
}