/**
 * @brief IModelLoader.h
 */
#pragma once

#include <Loaders/ISettingsLoader.h>

#include <toml.hpp>

namespace chai::settings
{
    class TomlSettingsLoader final : public ISettingsLoader
    {
    public:
        ~TomlSettingsLoader() override = default;
        bool canLoad(std::string_view ext) const override;
        std::optional<SettingsAsset> decode(std::span<const uint8_t> bytes,
                                            const std::filesystem::path& baseDir) override;
        const char* name() const override;
    };
} // namespace chai::settings