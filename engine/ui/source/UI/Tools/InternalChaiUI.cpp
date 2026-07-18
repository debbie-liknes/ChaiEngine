#include <UI/Tools/InternalChaiUi.h>
#include <algorithm>
#include <imgui.h>

namespace
{
    chai::ui::FontManager g_fontManager;
}

namespace chai::ui
{
    void Text(const std::string& label)
    {
        ImGui::Text("%s", label.c_str());
    }

    void TextColored(const math::Vec4& color, const std::string& label)
    {
        ImGui::TextColored(ImVec4(color.x, color.y, color.z, color.w), "%s", label.c_str());
    }

    bool Button(const std::string& label)
    {
        return ImGui::Button(label.c_str());
    }

    void SameLine()
    {
        ImGui::SameLine();
    }

    bool Checkbox(const std::string& label, bool& value)
    {
        return ImGui::Checkbox(label.c_str(), &value);
    }

    bool BeginChildRegion(const std::string& name,
        const math::Vec2& size,
        bool border)
    {
        return ImGui::BeginChild(name.c_str(), ImVec2(size.x, size.y), border);
    }

    void EndChildRegion()
    {
        ImGui::EndChild();
    }

    float GetScrollY()
    {
        return ImGui::GetScrollY();
    }

    float GetScrollMaxY()
    {
        return ImGui::GetScrollMaxY();
    }

    void SetScrollHereY(float y)
    {
        ImGui::SetScrollHereY(y);
    }

    TreeNode::TreeNode(std::string_view label) : m_open(ImGui::TreeNode(label.data())) {}

    TreeNode ::~TreeNode()
    {
        if (m_open)
            ImGui::TreePop();
    }

    void loadFonts(const std::string& fontDirectory)
    {
        g_fontManager.loadFonts(fontDirectory);
    }

    void PushFont(FontWeight weight)
    {
        g_fontManager.push(weight);
    }

    void PopFont()
    {
        g_fontManager.pop();
    }

    void TextFilter::draw(const std::string& label, float width)
    {
        char temp[256];
        std::strncpy(temp, buffer_.c_str(), sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';

        ImGui::SetNextItemWidth(width);
        if (ImGui::InputText(label.c_str(), temp, sizeof(temp)))
            buffer_ = temp;
    }

    bool TextFilter::passFilter(const std::string& text) const
    {
        if (buffer_.empty())
            return true;
        // simple case-insensitive substring match
        auto it = std::search(
            text.begin(), text.end(), buffer_.begin(), buffer_.end(), [](char a, char b) {
                return std::tolower(a) == std::tolower(b);
            });
        return it != text.end();
    }

    void TextFilter::clear()
    {
        buffer_.clear();
    }
} // namespace chai::ui