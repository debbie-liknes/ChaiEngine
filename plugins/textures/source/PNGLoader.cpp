#include <PNGLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Log.h>

namespace chai
{
    bool PNGLoader::canLoad(std::string_view ext) const
    {
        return ext == "png";
    }

    std::optional<TextureAsset> PNGLoader::decode(std::span<const uint8_t> bytes) const
    {
        if (bytes.empty())
            return std::nullopt;

        if (bytes.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
            CHAI_LOG_ERROR("Image too large for stb_image: {} bytes", bytes.size());
            return std::nullopt;
        }

        int w = 0, h = 0, srcChannels = 0;
        stbi_uc* decoded = stbi_load_from_memory(
            bytes.data(), static_cast<int>(bytes.size()), &w, &h, &srcChannels, STBI_rgb_alpha);

        if (!decoded) {
            CHAI_LOG_ERROR("stb_image decode failed: {}", stbi_failure_reason());
            return std::nullopt;
        }

        TextureAsset out;
        out.width = static_cast<uint32_t>(w);
        out.height = static_cast<uint32_t>(h);
        out.channels = 4; 

        const size_t byteCount = static_cast<size_t>(w) * h * 4;
        out.pixels.assign(decoded, decoded + byteCount);

        stbi_image_free(decoded);
        return out;
    }

    const char* PNGLoader::name() const
    {
        return "PNGLoader";
    }
}