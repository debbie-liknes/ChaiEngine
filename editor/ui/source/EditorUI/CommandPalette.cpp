#include <EditorUI/CommandPalette.h>

#include <Log.h>

#include <imgui.h>

namespace chai::ui
{
    struct CommandItem {
        std::string title;
        std::string actionId;
        std::string shortcut;
    };

    bool DrawCommandItem(const CommandItem& item, bool isSelected, ImGuiSelectableFlags flags = 0)
    {
        ImGui::PushID(item.actionId.c_str());

        bool clicked = ImGui::Selectable(
            "##selectable", isSelected, ImGuiSelectableFlags_SpanAllColumns | flags, ImVec2(0, 0));

        ImVec2 minPos = ImGui::GetItemRectMin();
        ImVec2 maxPos = ImGui::GetItemRectMax();

        ImGui::SameLine(minPos.x - ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x);
        if (!item.title.empty())
            ImGui::TextUnformatted(item.title.c_str());

        ImGui::SameLine();
        ImGui::TextColored(
            ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "(%s)", item.actionId.c_str());

        if (!item.shortcut.empty()) {
            float shortcutWidth = ImGui::CalcTextSize(item.shortcut.c_str()).x;
            float rightPosX = (maxPos.x - ImGui::GetWindowPos().x) - shortcutWidth -
                              ImGui::GetStyle().ItemSpacing.x;

            ImGui::SameLine(rightPosX);
            ImGui::TextColored(
                ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "%s", item.shortcut.c_str());
        }

        ImGui::PopID();

        return clicked;
    }

    void drawCommandPalette(const ActionManager& manager)
    {
        static int item_selected_idx = -1;
        static int item_highlighted_idx = -1;

        static char searchText[256] = "";
        static ActionManager::SearchReturnType results = manager.fuzzySearch(searchText);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputTextWithHint("##search", "Search...", searchText, IM_ARRAYSIZE(searchText))) {
            results = manager.fuzzySearch(searchText);
        }
        if (ImGui::BeginListBox(
                "##search_results", ImVec2(-FLT_MIN, -FLT_MIN))) {
            for (int i = 0; i < results.size(); i++) {
                bool is_selected = (item_selected_idx == i);
                ImGuiSelectableFlags flags =
                    ImGuiSelectableFlags_AllowDoubleClick |
                    ((item_highlighted_idx == i)
                        ? ImGuiSelectableFlags_Highlight
                        : 0);
                if (DrawCommandItem({results[i].second->getLabel(),
                                     results[i].second->getID(),
                                     static_cast<std::string>(results[i].second->getShortcut())},
                        is_selected, flags)) {
                    item_selected_idx = i;
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        results[i].second->trigger();
                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation
                // focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }
}
