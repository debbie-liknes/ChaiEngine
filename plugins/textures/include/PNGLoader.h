#pragma once
#include <Loaders/ImageLoader.h>

namespace chai
{
	class PNGLoader : public ITextureLoader
	{
    public:
        bool canLoad(std::string_view ext) const override;
        std::optional<TextureAsset> decode(std::span<const uint8_t> bytes) const override;
        const char* name() const override;
	};
}