#include <UI/Editor/TreeNode.h>
#include <IconsFontAwesome7.h>
#include <imgui.h>

namespace chai::ui
{
    ImGuiTreeNodeFlags toImGuiFlags(chai::ui::TreeNodeFlags flags)
    {
        ImGuiTreeNodeFlags out = ImGuiTreeNodeFlags_FramePadding;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::Leaf))
            out |= ImGuiTreeNodeFlags_Leaf;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::Selected))
            out |= ImGuiTreeNodeFlags_Selected;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::SpanFullWidth))
            out |= ImGuiTreeNodeFlags_SpanAvailWidth;
        return out;
    }

    TreeNode::TreeNode(std::string_view label,
                       std::string_view id,
                       std::string_view icon,
                       TreeNodeFlags flags)
    {
        ImGui::PushID(id.data());
        bool isLeaf = hasFlag(flags, TreeNodeFlags::Leaf);
        std::string fullId = "##" + std::string(id);
        ImGuiID imguiId = ImGui::GetID(fullId.c_str());

        ImVec2 rowStart = ImGui::GetCursorScreenPos();
        float rowMidY = rowStart.y + ImGui::GetTextLineHeight() * 0.5f;

        if (hasFlag(flags, TreeNodeFlags::DrawGuideLine)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImU32 lineColor = ImGui::GetColorU32(ImGuiCol_Border);
            float stubX = rowStart.x - ImGui::GetStyle().IndentSpacing * 1.5f;

            // horizontal stub: |_  pointing into this row's icon
            drawList->AddLine(ImVec2(stubX, rowMidY), ImVec2(rowStart.x, rowMidY), lineColor, 1.0f);
            // vertical tick: connects up toward whatever drew above this row at the same depth
            drawList->AddLine(ImVec2(stubX, rowStart.y - ImGui::GetStyle().ItemSpacing.y),
                              ImVec2(stubX, rowMidY),
                              lineColor,
                              1.0f);
        }

        if (isLeaf) {
            open_ = false;
            std::string fullLabel = std::string(icon) + "  " + std::string(label);
            clicked_ = ImGui::Selectable((fullLabel + fullId).c_str(),
                                         hasFlag(flags, TreeNodeFlags::Selected));
            ImGui::PopID();
            return;
        }

        ImGuiStorage* storage = ImGui::GetStateStorage();
        bool wasOpen = storage->GetBool(imguiId, hasFlag(flags, TreeNodeFlags::DefaultOpen));
        const char* chevron = wasOpen ? ICON_FA_CHEVRON_DOWN : ICON_FA_CHEVRON_RIGHT;
        std::string fullLabel =
            std::string(chevron) + " " + std::string(icon) + "  " + std::string(label);

        clicked_ = ImGui::Selectable((fullLabel + fullId).c_str(),
                                     hasFlag(flags, TreeNodeFlags::Selected));
        if (clicked_) {
            wasOpen = !wasOpen;
            storage->SetBool(imguiId, wasOpen);
        }
        open_ = wasOpen;
        ImGui::PopID();
    }

    TreeNode::~TreeNode()
    {
/*        if (open_)
            ImGui::TreePop();*/
    }
}