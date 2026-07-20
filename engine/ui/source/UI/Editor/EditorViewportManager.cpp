#include <UI/Editor/EditorViewportManager.h>
#include <UI/Editor/PanelRegistry.h>
#include <imgui.h>

namespace chai::ui
{
    EditorViewportManager::EditorViewportManager(gfx::IRenderer& renderer,
                                                 PanelRegistry& panelRegistry)
        : panelRegistry_(panelRegistry), renderer_(renderer)
    {
    }

    void drawViewportPanel(gfx::IRenderer& renderer, gfx::ViewportHandle handle)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        renderer.requestViewportResize(handle, (uint32_t)avail.x, (uint32_t)avail.y);
        ImGui::Image(renderer.getViewportTextureId(handle), avail);
        renderer.setViewportHovered(handle, ImGui::IsItemHovered());
    }

    std::string EditorViewportManager::addViewport(const std::string& name, uint32_t cameraViewId)
    {
        //create the handle
        auto handle = renderer_.addViewport(name, cameraViewId);

        //tell the system about the panel
        ui::PanelDesc panelInfo;
        panelInfo.displayName = name;
        panelInfo.id = name + "##Viewport_" + std::to_string(renderer_.getViewportTextureId(handle));
        panelInfo.draw = [this, handle] { drawViewportPanel(renderer_, handle); };
        panelInfo.visible = true;
        panelRegistry_.registerPanel(panelInfo);

        return panelInfo.id;
    }
}
