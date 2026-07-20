#include <UI/Editor/TreeNode.h>
#include <imgui.h>

namespace chai::ui
{
    ImGuiTreeNodeFlags toImGuiFlags(chai::ui::TreeNodeFlags flags)
    {
        ImGuiTreeNodeFlags out = ImGuiTreeNodeFlags_FramePadding;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::Leaf))
            out |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::Selected))
            out |= ImGuiTreeNodeFlags_Selected;
        if (hasFlag(flags, chai::ui::TreeNodeFlags::SpanFullWidth))
            out |= ImGuiTreeNodeFlags_SpanAvailWidth;
        return out;
    }

    TreeNode::TreeNode(std::string_view label, std::string_view id, TreeNodeFlags flags)
    {
        if (hasFlag(flags, TreeNodeFlags::DefaultOpen))
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);

        std::string fullId = "##" + std::string(id);
        open_ = ImGui::TreeNodeEx(
            fullId.c_str(), toImGuiFlags(flags), "%s", std::string(label).c_str());
        clicked_ = ImGui::IsItemClicked();
    }

    TreeNode::~TreeNode()
    {
        if (open_)
            ImGui::TreePop();
    }
}