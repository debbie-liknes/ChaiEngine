#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/MenuConfigLoader.h>
#include <Containers/Dictionary.h>
#include <Action/Action.h>

namespace chai::ui
{
    class MenuService
    {
    public:
        explicit MenuService(const std::filesystem::path& configFile);

        void registerAction(const std::string& name, const std::function<void()>& action);
        void unregisterAction(const std::string& name);

        void draw(PanelRegistry& registry);


    private:
        struct Node {
            std::unique_ptr<Action> action;
            std::vector<Node> children;
        };
        Node root_;

        std::unique_ptr<MenuConfigLoader> loader_;
        std::unordered_map<std::string, Action*> actionMap_;

        void buildMenu();
        Node convertToNodeDFS(const BlueprintItemSchema& schema, const std::string& label);
        void drawNode(const MenuService::Node& node, PanelRegistry& registry);
    };
} // namespace chai::ui