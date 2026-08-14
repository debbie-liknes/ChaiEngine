#include <EditorUI/EditorMenu.h>

#include <imgui.h>

namespace chai::ui
{
    void drawAction(Action& action, ActionManager& manager, PanelRegistry& registry)
    {
        if (action.isSeparator()) {
            ImGui::Separator();
        } else if (!action.getChildren()->empty()) {
            if (ImGui::BeginMenu(action.getLabel().c_str(),
                                    action.isEnabled())) {
                for (auto& child : *action.getChildren())
                    drawAction(*child.get(), manager, registry);
                ImGui::EndMenu();
            }
        } else if (action.getLabel().size()) {
            if (ImGui::MenuItem(action.getLabel().c_str(),
                                std::string(action.getShortcut()).c_str(),
                                    registry.isVisible(
                                    manager.getPanelId(action.getID())),
                                action.isEnabled())) {
                action.trigger();
                action.toggle();
            }
        }
    }

    void drawEditorMenu(ActionManager& manager, PanelRegistry& registry)
    {
        if (!ImGui::BeginMainMenuBar())
            return;
        for (auto& child : *manager.getAction("editor")->getChildren())
            drawAction(*child.get(), manager, registry);
        ImGui::EndMainMenuBar();
    }
} // namespace chai::ui
