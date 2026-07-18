#include <UI/Tools/InternalPanels.h>
#include <imgui.h>
#include <UI/Tools/InternalChaiUi.h>

namespace chai::ui
{
    struct Panel {
        PanelDrawFn draw;
        bool visible;
    };
    static std::unordered_map<std::string, Panel> s_panels;

    void registerPanel(const std::string& name, PanelDrawFn fn, bool startVisible)
    {
        s_panels[name] = {std::move(fn), startVisible};
    }

    void unregisterPanel(const std::string& name)
    {
        s_panels.erase(name);
    }

    void setPanelVisible(const std::string& name, bool visible)
    {
        if (auto it = s_panels.find(name); it != s_panels.end())
            it->second.visible = visible;
    }

    void beginDockspace()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("DockspaceHost", nullptr, hostFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
        ImGui::DockSpace(dockspaceId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }

    void drawRegisteredPanels()
    {
        beginDockspace();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Panels")) {
                for (auto& [name, panel] : s_panels)
                    ImGui::MenuItem(name.c_str(), nullptr, &panel.visible);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        for (auto& [name, panel] : s_panels) {
            if (!panel.visible)
                continue;
            ui::PushFont(ui::FontWeight::BoldTitle);
            bool began = ImGui::Begin(name.c_str(), &panel.visible, ImGuiWindowFlags_NoCollapse);
            ui::PopFont();

            if (began)
                panel.draw();
            ImGui::End();
        }
    }
} // namespace chai::ui