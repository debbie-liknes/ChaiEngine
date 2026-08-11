#pragma once

#include <string>
#include <filesystem>
#include <optional>

#include <Action/Action.h>

namespace chai::ui
{    
    // Represents an individual item (menu, action, or separator)
    struct BlueprintItemSchema {
        std::string type;                       // "menu", "action", or "separator"
        std::optional<std::string> id;          // Optional for separators
        std::optional<std::string> label;       // Optional for separators
        std::optional<std::string> shortcut;    // Optional
        std::vector<BlueprintItemSchema> items; // Recursive children for "menu" types

        
    };

    struct ActionConfigData {
        std::vector<BlueprintItemSchema> editor;
    };

    class ActionConfigLoader
    {
    public:
        explicit ActionConfigLoader(const std::filesystem::path& configFile);

        ActionConfigData getConfig() const { return config_; }
    private:
        void parseConfigFile(const std::filesystem::path& configFile);

        ActionConfigData config_;
    };
} // namespace chai::ui
