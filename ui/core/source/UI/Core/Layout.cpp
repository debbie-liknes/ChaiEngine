#include <UI/Core/Layout.h>
#include <imgui.h>

namespace chai::ui
{
    math::Vec2 calcText(const std::string& text)
    {
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        return { textSize.x, textSize.y };
    }
}