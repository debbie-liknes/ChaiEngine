#pragma once
#include <Assets/ModelAsset.h>
#include <optional>
#include <span>
#include <string_view>
#include <filesystem>

namespace chai::gfx
{
    class IModelLoader
    {
    public:
        virtual ~IModelLoader() = default;
        virtual bool canLoad(std::string_view ext) const = 0;
        virtual std::optional<ModelAsset> decode(std::span<const uint8_t> bytes,
                                                 const std::filesystem::path& baseDir) = 0;
        virtual const char* name() const = 0;
    };
} // namespace chai::gfx