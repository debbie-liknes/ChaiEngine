#pragma once
#include <Assets/TextureAsset.h>
#include <optional>
#include <string_view>
#include <span>

namespace chai::gfx
{
    // bytes -> image asset
    class ITextureLoader
    {
    public:
        virtual ~ITextureLoader() = default;
        virtual bool canLoad(std::string_view ext) const = 0;
        virtual std::optional<TextureAsset> decode(std::span<const uint8_t> bytes) const = 0;
        virtual const char* name() const = 0;
    };
}