/**
 * @file ITextureRegistry.h
 */
#pragma once
#include <Handle.h>
#include <filesystem>
#include <Assets/TextureAsset.h>
#include <array>

namespace chai::gfx
{
    /**
     * @brief Bookkeeping for CPU texture data
     */
    class ITextureRegistry
    {
    public:
        virtual ~ITextureRegistry() = default;

        virtual Handle<Texture> ingest(AssetId, TextureAsset) = 0;
        virtual Handle<Texture> load(AssetId, const std::filesystem::path&) = 0;
        virtual Handle<Texture> loadCubemap(AssetId, const std::array<std::filesystem::path, 6>&) = 0;
        virtual void release(Handle<Texture>) = 0;
    };
} // namespace chai