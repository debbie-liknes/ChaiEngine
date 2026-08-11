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
    class ActionManager;
    class PanelRegistry;

    class MenuService
    {
    public:
        explicit MenuService(const std::filesystem::path& configFile,
                             ActionManager* manager,
                             PanelRegistry* registry);

        void draw();


    private:
        struct Node {
            std::shared_ptr<Action> action;
            std::vector<Node> children;
        };
        Node root_;

        std::unique_ptr<MenuConfigLoader> loader_;
        ActionManager* actionManager_ = nullptr;
        PanelRegistry* panelRegistry_ = nullptr;
        Dictionary<Action*> actionMap_;

        void buildMenu();
        Node convertToNodeDFS(const BlueprintItemSchema& schema, const std::string& label);
        void drawNode(const MenuService::Node& node);
    };
} // namespace chai::ui