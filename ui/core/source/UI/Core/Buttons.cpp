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

    bool drawButton(const Rect& r, const ButtonDesc& d)
    {
        if (r.w <= 0.f || r.h <= 0.f)
            return false;

        std::string idStr = !d.id.empty() ? d.id : d.label;
        ImGui::SetCursorScreenPos({r.min().x, r.min().y});
        ImGui::InvisibleButton(idStr.c_str(), {r.w, r.h});

        const ImGuiID id = ImGui::GetItemID();
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();
        const bool clicked = ImGui::IsItemClicked();

        ImDrawList* dl = ImGui::GetWindowDrawList();

        // styling comes from base imgui, but we could have our own themes
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 bg = style.Colors[ImGuiCol_Button];
        if (clicked || held) {
            bg = style.Colors[ImGuiCol_ButtonActive];
        }
        else if (hovered) {
            bg = style.Colors[ImGuiCol_ButtonHovered];
        }
        ImVec4 borderColor = style.Colors[ImGuiCol_Border];
        float rounding = style.FrameRounding;

        //draw background
        dl->AddRectFilled({r.min().x, r.min().y}, {r.max().x, r.max().y}, ImGui::GetColorU32(bg), rounding);
        dl->AddRect({r.min().x, r.min().y}, {r.max().x, r.max().y}, ImGui::GetColorU32(borderColor), rounding);

        ImVec4 fg = style.Colors[ImGuiCol_Text];
        ImVec2 textSize = ImGui::CalcTextSize(d.label.c_str());

        //layout calcs (a layout engine would help?)
        float x = r.x + (r.w - textSize.x) * 0.5f;
        float cy = r.y + r.h * 0.5f;

        dl->AddText({x, cy - textSize.y * 0.5f}, ImGui::GetColorU32(fg), d.label.c_str());

        return clicked;
    }

    // =========================================================
    // Wrappers
    // =========================================================

    bool button(const std::string& label)
    {
        return ImGui::Button(label.c_str());
    }

    bool button(const ButtonDesc& desc)
    {
        const Measured m = measureButton(desc.label);
        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const Rect r{cursor.x, cursor.y, m.desired.w, m.desired.h};

        ImGui::Dummy({m.desired.w, m.desired.h});
        return drawButton(r, desc);
    }

    bool checkbox(const std::string& label, bool* val)
    {
        return ImGui::Checkbox(label.c_str(), val);
    }
} // namespace chai::ui