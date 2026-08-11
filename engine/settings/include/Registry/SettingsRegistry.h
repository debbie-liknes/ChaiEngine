#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>
#include <any>
#include <memory>
#include <AssetCache.h>


namespace chai::settings
{
    class SettingsRegistry final
    {
    public:
        void load(AssetId id, const std::filesystem::path& path);

        void release(AssetId id);

    private:
        std::unordered_map<std::string, std::any> registry_;
    };
} // namespace chai::settings
