#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <UI/Editor/PanelRegistry.h>

namespace chai::ui
{
    //TODO: omg, events
    struct TogglePanel {
        std::string panelId;
    };
    struct MenuAction {
        std::function<void()> onClick;
    };
    struct MenuSeparator {
    };

    using MenuLeaf = std::variant<TogglePanel, MenuAction, MenuSeparator>;

    class MenuService
    {
    public:
        void registerItem(const std::string& path, MenuLeaf leaf);
        void unregisterItem(const std::string& path);

        void draw(PanelRegistry& registry);

    private:
        struct Node {
            std::string label;
            std::optional<MenuLeaf> leaf;
            std::vector<Node> children;
        };
        Node root_;

        Node& findOrCreate(Node& parent, const std::vector<std::string>& segments, size_t idx);
        void drawNode(const MenuService::Node& node, PanelRegistry& registry);
    };
} // namespace chai::ui