#include <EditorUI/PluginManager.h>
#include <UI/Core/FontManager.h>
#include <UI/Core/InternalChaiUI.h>
#include <imgui.h>

namespace chai
{
    void drawPluginManager(const PluginLoader& loader)
    {
        static int selectedIndex = -1;

        auto plugins = loader.loadedPluginInfo();

        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                                ImGuiTableFlags_SizingStretchSame;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(15.0f, 0.0f));
        if (ImGui::BeginTable("SplitLayout", 2, flags)) {
            // -------------------------------------------------------------------
            // COLUMN 1: List View (Left Pane)
            // -------------------------------------------------------------------
            ImGui::TableNextColumn();

            ImGui::TextDisabled("PLUGINS");
            ImGui::Separator();

            // Wrap list in a child window so it scrolls independently
            ImGui::BeginChild("ListPane", ImVec2(0, 0), false);
            for (int i = 0; i < plugins.size(); i++) {
                // Render selectable list item
                if (ImGui::Selectable(plugins[i].library.getName().c_str(), selectedIndex == i)) {
                    selectedIndex = i;
                }
            }
            ImGui::EndChild();

            // -------------------------------------------------------------------
            // COLUMN 2: Metadata Pane (Right Pane)
            // -------------------------------------------------------------------
            ImGui::TableNextColumn();

            ImGui::TextDisabled("METADATA");
            if (selectedIndex >= 0 && selectedIndex < plugins.size() &&
                plugins[selectedIndex].library.isReloadable()) {
                ImGui::SetCursorPosX(
                    ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x -
                    (ImGui::CalcTextSize("Reload").x + ImGui::GetStyle().FramePadding.x * 2.0f));
                ImGui::SameLine();
                if (ImGui::Button("Reload")) {
                    // TODO: Reload plugin functionality
                }
            }
            ImGui::Separator();

            // Wrap properties in a child window for independent scrolling
            ImGui::BeginChild("PropertiesPane", ImVec2(0, 0), false);

            if (selectedIndex >= 0 && selectedIndex < plugins.size()) {
                const DynamicLibrary& lib = plugins[selectedIndex].library;

                ImGui::Text("Name: %s", lib.getName().c_str());
                ImGui::Text("Author: %s", lib.getAuthor().c_str());
                ImGui::Text("Version: %s", lib.getVersion().c_str());
                ImGui::Text("Binary: %s", lib.getBinaryPath().c_str());
                ImGui::Text("Development Library: %s", lib.isDevelopment() ? "true" : "false");
                ImGui::Text("Checksum: %s", lib.getChecksum().c_str());
            } else {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                                   "Select a plugin from the list to view metadata.");
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }
} // namespace chai
