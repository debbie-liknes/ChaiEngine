#pragma once
#include <Loaders/ITextureLoader.h>

namespace chai
{
	class PNGLoader : public gfx::ITextureLoader
	{
    public:
        bool canLoad(std::string_view ext) const override;
        std::optional<gfx::TextureAsset> decode(std::span<const uint8_t> bytes,
                                                const gfx::TextureFormat& format) const override;
        const char* name() const override;
	};
}