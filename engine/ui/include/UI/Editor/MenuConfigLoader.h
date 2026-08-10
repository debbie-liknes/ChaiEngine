#pragma once

#include <string>
#include <filesystem>

namespace chai::ui
{
    //enum class MenuItemType
    //{
    //    MENU,
    //    ACTION,
    //    SEPARATOR
    //};

    //struct BaseMenuDefinition
    //{
    //    MenuItemType type;
    //};

    //struct ActionDefinition : public BaseMenuDefinition
    //{
    //    std::string id;
    //    std::string label;
    //};

    //struct MenuDefinition : public ActionDefinition
    //{
    //    std::vector<BaseMenuDefinition*> children;
    //};

    
    // Represents an individual item (menu, action, or separator)
    struct BlueprintItemSchema {
        std::string type;                       // "menu", "action", or "separator"
        std::optional<std::string> id;          // Optional for separators
        std::optional<std::string> label;       // Optional for separators
        std::vector<BlueprintItemSchema> items; // Recursive children for "menu" types
    };

    struct MenuConfigData {
        std::vector<BlueprintItemSchema> editor;
    };

    class MenuConfigLoader
    {
    public:
        explicit MenuConfigLoader(const std::filesystem::path& configFile);

        MenuConfigData getConfig() const { return config_; }
    private:
        void parseConfigFile(const std::filesystem::path& configFile);

        MenuConfigData config_;
    };
} // namespace chai::ui
