#include <UI/Editor/EditorViewportManager.h>
#include <UI/Editor/PanelRegistry.h>
#include <imgui.h>

namespace chai::ui
{
    EditorViewportManager::EditorViewportManager(gfx::IViewportRegistry& registry,
                                                 PanelRegistry& panelRegistry)
        : panelRegistry_(panelRegistry), registry_(registry)
    {
    }

    void drawViewportOverlay(gfx::IViewportRegistry& registry,
                             gfx::ViewportHandle handle,
                             ImVec2 imageOrigin,
                             ImVec2 imageSize)
    {
        ImGui::SetCursorScreenPos(ImVec2(imageOrigin.x + 8, imageOrigin.y + 8));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0.55f));
        ImGui::BeginChild("##ViewportToolbar",
                          ImVec2(imageSize.x - 16, ImGui::GetFontSize() * 3),
                          true,
                          ImGuiWindowFlags_NoScrollbar);

        static const char* kShadingNames[] = {"Lit", "Normals", "UV", "Overdraw"};
        int shadingMode = static_cast<int>(registry.getViewportShadingMode(handle));
        ImGui::SetNextItemWidth(100);
        if (ImGui::Combo("##Shading", &shadingMode, kShadingNames, IM_ARRAYSIZE(kShadingNames)))
            registry.setViewportShadingMode(handle,
                                            static_cast<gfx::ViewportShadingMode>(shadingMode));

        ImGui::SameLine();

        bool wireframe = registry.isViewportWireframe(handle);
        if (ImGui::Checkbox("Wireframe", &wireframe))
            registry.setViewportWireframe(handle, wireframe);

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }  

    void drawViewportPanel(gfx::IViewportRegistry& registry, gfx::ViewportHandle handle)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        registry.requestViewportResize(handle, (uint32_t)avail.x, (uint32_t)avail.y);

        ImVec2 imageOrigin = ImGui::GetCursorScreenPos();
        ImGui::Image(registry.getViewportTextureId(handle), avail);
        registry.setViewportHovered(handle, ImGui::IsItemHovered());

        drawViewportOverlay(registry, handle, imageOrigin, avail);
    }

    std::string EditorViewportManager::addViewport(const std::string& name, uint32_t cameraViewId)
    {
        //create the handle
        auto handle = registry_.addViewport(name, cameraViewId);

        //tell the system about the panel
        ui::PanelDesc panelInfo;
        panelInfo.displayName = name;
        panelInfo.id = name + "##Viewport_" + std::to_string(registry_.getViewportTextureId(handle));
        panelInfo.draw = [this, handle] { drawViewportPanel(registry_, handle); };
        panelInfo.visible = true;
        panelRegistry_.registerPanel(panelInfo);

        return panelInfo.id;
    }
}
