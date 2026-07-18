#include <UI/Tools/InternalChaiUi.h>
#include <imgui.h>

namespace chai::ui
{
    void Text(const std::string& label)
    {
        ImGui::Text("%s", label.c_str());
    }

    bool Button(const std::string& label)
    {
        return ImGui::Button(label.c_str());
    }

    TreeNode::TreeNode(std::string_view label) : m_open(ImGui::TreeNode(label.data())) {}

    TreeNode ::~TreeNode()
    {
        if (m_open)
            ImGui::TreePop();
    }
}