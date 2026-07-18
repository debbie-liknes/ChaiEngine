#include <UI/Tools/InternalPanels.h>
#include <imgui.h>

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

    void drawRegisteredPanels()
    {
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
            if (ImGui::Begin(name.c_str(), &panel.visible))
                panel.draw();
            ImGui::End();
        }
    }
} // namespace chai::ui