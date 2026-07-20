#include <UI/Editor/PanelHost.h>
#include <UI/Editor/InternalChaiUI.h>
#include <imgui.h>

namespace chai::ui
{
    void PanelHost::draw(PanelRegistry& registry, DockspaceService& dockspace, MenuService& menus)
    {
        menus.draw(registry);
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
    }
}