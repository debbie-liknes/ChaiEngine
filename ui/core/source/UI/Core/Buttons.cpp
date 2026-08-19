#include <UI/Core/Buttons.h>
#include <UI/Core/Layout.h>
#include <imgui.h>

namespace chai::ui
{
    // =========================================================
    // Layout
    // =========================================================
    Measured measureButton(const std::string& label)
    {
        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
        ImVec2 padding = ImGui::GetStyle().FramePadding;

        Measured m;
        m.desired.w = textSize.x + padding.x * 2.f;
        m.desired.h = textSize.y + padding.y * 2.f;

        return m;
    }

    // =========================================================
    // Draw
    // =========================================================

    // =========================================================
    // Wrappers
    // =========================================================

    bool button(const std::string& label)
    {
        return ImGui::Button(label.c_str());
    }

    bool checkbox(const std::string& label, bool* val)
    {
        return ImGui::Checkbox(label.c_str(), val);
    }



    /////////////////////////////////////////////////////////////////
    Button::Button(const std::string& label) : label_(label)
    {

    }

    void Button::draw(UIContext& context) 
    {
        if (!visible)
            return;

        if (ImGui::Button(label_.c_str())) {
            if (onClick)
                onClick();
        }
    }
} // namespace chai::ui