#include <UI/Tools/EditorViewportManager.h>
#include <UI/Editor/PanelRegistry.h>
#include <imgui.h>
#include <Rendering/Viewport.h>

namespace chai::ui
{
    EditorViewportManager::EditorViewportManager(gfx::IRenderer& renderer,
                                                      PanelRegistry& panelRegistry): panelRegistry_(panelRegistry)
    {
    }

    void drawViewportPanel(gfx::IRenderer& renderer, gfx::ViewportHandle handle)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        renderer.requestViewportResize(handle, (uint32_t)avail.x, (uint32_t)avail.y);
        ImGui::Image(renderer.getViewportTextureId(handle), avail);
        renderer.setViewportHovered(handle, ImGui::IsItemHovered());
    }

    std::string EditorViewportManager::addPane(gfx::IRenderer& renderer,
                                      const std::string& name,
                                      uint32_t cameraViewIndex)
    {
        //create the handle
        auto handle = renderer.addViewport(name, cameraViewIndex);

        //tell the system about the panel
        ui::PanelDesc panelInfo;
        panelInfo.displayName = name;
        panelInfo.id = name + "##Viewport_" + std::to_string(renderer.getViewportTextureId(handle));
        panelInfo.draw = [this, &renderer, handle] { drawViewportPanel(renderer, handle); };
        panelInfo.visible = true;
        panelRegistry_.registerPanel(panelInfo);

        return panelInfo.id;
    }
}
