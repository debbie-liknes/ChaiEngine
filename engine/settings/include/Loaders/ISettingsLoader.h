/**
 * @brief IModelLoader.h
 */
#pragma once
#include <optional>
#include <span>
#include <string_view>
#include <filesystem>

#include <Assets/SettingsAsset.h>

namespace chai::settings
{
    /**
     * @brief Inherit from this in a plugin to load in a settings file
     */
    class ISettingsLoader
    {
    public:
        virtual ~ISettingsLoader() = default;
        virtual bool canLoad(std::string_view ext) const = 0;
        virtual std::optional<SettingsAsset> decode(std::span<const uint8_t> bytes,
                                                 const std::filesystem::path& baseDir) = 0;
        virtual const char* name() const = 0;
    };
} // namespace chai::settings