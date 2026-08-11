#include <UI/Editor/MenuService.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <sstream>
#include <algorithm>
#include <stack>
#include <UI/Editor/ActionManager.h>
#include <UI/Editor/PanelRegistry.h>

namespace chai::ui
{
    MenuService::MenuService(const std::filesystem::path& configFile, ActionManager* manager, PanelRegistry* registry)
        : loader_(std::make_unique<MenuConfigLoader>(configFile)), actionManager_(manager),
          panelRegistry_(registry)
    {
        buildMenu();
    }

    MenuService::Node MenuService::convertToNodeDFS(const BlueprintItemSchema& schema, const std::string& slug)
    {
        Node node;
        if (schema.id.has_value()) {
            const auto id = (slug.size() > 0 ? slug + "." : "") + schema.id.value();
            node.action = actionManager_->getOrCreateAction(id);
            if (schema.label.has_value())
                node.action->setLabel(schema.label.value());
            if (schema.shortcut.has_value()) {
                if (auto shortcutMaybe = Shortcut::fromString(schema.shortcut.value());
                    shortcutMaybe.has_value()) {
                    node.action->setShortcut(shortcutMaybe.value());
                }
            }
        } else if (schema.type == "separator") {
            // Separator item is a special case -- we'll return early here if encountered.
            node.action = std::make_shared<Action>();
            node.action->setSeparator(true);
            return node;
        }

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

    void MenuService::drawNode(const MenuService::Node& node)
    {
        for (auto& child : node.children) {
            if (child.action) {
                if (child.action->isSeparator()) {
                    ImGui::Separator();
                } else if (!child.children.empty()) {
                    if (ImGui::BeginMenu(child.action->getLabel().c_str(), child.action->isEnabled())) {
                        drawNode(child);
                        ImGui::EndMenu();
                    }
                } else if (child.action->getLabel().size()) {
                    if (ImGui::MenuItem(child.action->getLabel().c_str(),
                                        std::string(child.action->getShortcut()).c_str(),
                                        panelRegistry_->isVisible(actionManager_->getPanelId(child.action->getID())),
                                        child.action->isEnabled())) {
                        child.action->trigger();
                        child.action->toggle();
                    }
                }
            }
        }
    }

    void MenuService::draw()
    {
        if (!ImGui::BeginMainMenuBar())
            return;
        drawNode(root_);
        ImGui::EndMainMenuBar();
    }
} // namespace chai::ui
