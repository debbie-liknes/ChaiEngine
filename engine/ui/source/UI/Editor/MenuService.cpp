#include <UI/Editor/MenuService.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <sstream>
#include <algorithm>
#include <stack>

namespace chai::ui
{
    MenuService::MenuService(const std::filesystem::path& configFile)
        : loader_(std::make_unique<MenuConfigLoader>(configFile))
    {
        buildMenu();
    }

    MenuService::Node MenuService::convertToNodeDFS(const BlueprintItemSchema& schema, const std::string& slug)
    {
        Node node;
        node.action = std::make_unique<Action>();
        if (schema.id.has_value())
            node.action->setID((slug.size() > 0 ? slug + "." : "") + schema.id.value());
        if (schema.label.has_value())
            node.action->setLabel(schema.label.value());

        node.children.reserve(schema.items.size());
        for (const auto& childSchema : schema.items) {
            if (schema.id.has_value()) {
                node.children.push_back(convertToNodeDFS(childSchema, node.action->getID()));
            } else {
                node.children.push_back(convertToNodeDFS(childSchema, ""));
            }
        }

        if (schema.id.has_value()) {
            actionMap_[node.action->getID()] = node.action.get();
        }

        return node;
    }

    void MenuService::buildMenu()
    {
        auto config = loader_->getConfig();
        for (const auto& item : config.editor) {
            root_.children.push_back(convertToNodeDFS(item, ""));
        }
    }

    void MenuService::registerAction(const std::string& name, const std::function<void()>& action)
    {
        if (auto actionItr = actionMap_.find(name); actionItr != actionMap_.end()) {
            actionItr->second->registerAction(action);
        }
    }

    void MenuService::unregisterAction(const std::string& name)
    {
        if (auto actionItr = actionMap_.find(name); actionItr != actionMap_.end()) {
            actionItr->second->registerAction(nullptr);
        }
    }

    void MenuService::drawNode(const MenuService::Node& node, PanelRegistry& registry)
    {
        for (auto& child : node.children) {
            if (child.action) {
                if (!child.children.empty()) {
                    if (ImGui::BeginMenu(child.action->getLabel().c_str())) {
                        drawNode(child, registry);
                        ImGui::EndMenu();
                    }
                } else if (child.action->getLabel().size()) {
                    if (ImGui::MenuItem(child.action->getLabel().c_str(),
                                        nullptr,
                                        registry.isVisible(child.action->getID()),
                                        child.action->isEnabled())) {
                        child.action->trigger();
                        child.action->toggle();
                    }
                }
            }
        }
    }

    void MenuService::draw(PanelRegistry& registry)
    {
        if (!ImGui::BeginMainMenuBar())
            return;
        drawNode(root_, registry);
        ImGui::EndMainMenuBar();
    }
} // namespace chai::ui
