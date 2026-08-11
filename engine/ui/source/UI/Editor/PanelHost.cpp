#include <UI/Editor/PanelHost.h>
#include <UI/Editor/InternalChaiUI.h>
#include <imgui.h>
#include <tracy/Tracy.hpp>

namespace chai::ui
{
    void PanelHost::draw(PanelRegistry& registry, DockspaceService& dockspace, MenuService& menus)
    {
        ZoneScoped

        menus.draw();
        ImGuiID dockId = dockspace.begin();

        for (PanelDesc& panel : registry.panels()) {
            if (!panel.visible)
                continue;
            //ui::PushFont(panel.titleFont);
            bool began =
                ImGui::Begin(panel.displayName.c_str(), &panel.visible, ImGuiWindowFlags_NoCollapse);
            //ui::PopFont();
            if (began)
                panel.draw();
            ImGui::End();
        }
    }
}