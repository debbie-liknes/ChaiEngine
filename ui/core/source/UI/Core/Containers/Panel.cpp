#include <UI/Core/Containers/Panel.h>
#include <imgui.h>

namespace chai::ui
{
    Panel::Panel(const std::string& name) : displayName_(name)
    {
        
    }

    void Panel::draw(UIContext& context)
    {
        if (!visible)
            return;

        bool began = ImGui::Begin(displayName_.c_str(), &visible, ImGuiWindowFlags_NoCollapse);

        if (began)
            Container::draw(context);

        ImGui::End();
    }

    const std::string& Panel::displayName() const
    {
        return displayName_;
    }
}