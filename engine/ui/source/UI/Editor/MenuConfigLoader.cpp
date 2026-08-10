#include <UI/Editor/MenuConfigLoader.h>

#include <Log.h>
#include <json.hpp>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace chai::ui
{
    // Custom deserialization for recursive BlueprintItemSchema
    void from_json(const json& j, BlueprintItemSchema& item)
    {
        j.at("type").get_to(item.type);

        if (j.contains("id") && !j["id"].is_null()) {
            item.id = j.at("id").get_to(item.id.emplace());
        }

        if (j.contains("label") && !j["label"].is_null()) {
            item.label = j.at("label").get_to(item.label.emplace());
        }

        if (j.contains("items") && j["items"].is_array()) {
            j.at("items").get_to(item.items);
        }
    }

    // Custom deserialization for MenuConfigData
    void from_json(const json& j, MenuConfigData& config)
    {
        j.at("editor").get_to(config.editor);
    }

    // Utility function to recursively print and verify the parsed structure
    void printBlueprintTree(const BlueprintItemSchema& item, int indentLevel = 0)
    {
        std::string indent(indentLevel * 2, ' ');

        if (item.type == "separator") {
            std::cout << indent << "--- [Separator] ---" << std::endl;
            return;
        }

        std::cout << indent << "[" << item.type << "] " << (item.label ? *item.label : "Unlabeled")
                  << " (ID: " << (item.id ? *item.id : "None") << ")" << std::endl;

        for (const auto& child : item.items) {
            printBlueprintTree(child, indentLevel + 1);
        }
    }

    MenuConfigLoader::MenuConfigLoader(const std::filesystem::path& configFile)
    {
        parseConfigFile(configFile);
    }

    void MenuConfigLoader::parseConfigFile(const std::filesystem::path& configFile)
    {
        try
        {
            std::ifstream configFileStream(configFile);
            json j = json::parse(configFileStream);

            config_ = j.get<MenuConfigData>();
                
            for (const auto& topLevelItem : config_.editor) {
                printBlueprintTree(topLevelItem);
            }
        }
        catch (json::exception& ex)
        {
            CHAI_LOG_ERROR("Menu config parsing error: {}", ex.what());
        }
    }
} // namespace chai::ui
