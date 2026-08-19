#include <EditorUI/PanelHost.h>
#include <UI/Core/InternalChaiUI.h>
#include <imgui.h>
#include <tracy/Tracy.hpp>
#include <EditorUI/EditorMenu.h>

namespace chai::ui
{
    void PanelHost::draw(PanelRegistry& registry, DockspaceService& dockspace, ActionManager& manager)
    {
        ZoneScoped

        drawEditorMenu(manager, registry);
        ImGuiID dockId = dockspace.begin();

        for (PanelDesc& panel : registry.panels()) {
            if (!panel.visible)
                continue;
            //ui::PushFont(panel.titleFont);
            bool began =
                ImGui::Begin(panel.id.c_str(), &panel.visible, ImGuiWindowFlags_NoCollapse);
            //ui::PopFont();
            if (began)
                panel.draw();
            ImGui::End();
        }

        UIContext uiUpdateCtx;
        for (auto&& panel : registry.panelsNew()) {
            panel->draw(uiUpdateCtx);
        }
    }
}