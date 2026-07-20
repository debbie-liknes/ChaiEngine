/**
 * @file InternalChaiUI.h
 * @brief UI for internals and tooling. NOT for production use.
 */
#pragma once
#include <string>
#include <vector>
#include <ChaiMath.h>
#include <UI/Editor/FontManager.h>

namespace chai::ui
{
    // this is imgui render backend interface. We will for now provide the implementation in the vulkan backend
    class IInternalChaiUi
    {
    public:
        virtual ~IInternalChaiUi() = default;
        virtual bool initializeUI() = 0;
        virtual void shutdownUI() = 0;
    };

    class TextFilter
    {
    public:
        void draw(const std::string& label = "Filter", float width = 0.0f);
        bool passFilter(const std::string& text) const;
        void clear();

    private:
        std::string buffer_;
    };

    void PushFont(FontWeight weight);
    void PopFont();
    void loadFonts(const std::string& fontDirectory);

    class ScopedFont
    {
    public:
        explicit ScopedFont(FontWeight weight) { chai::ui::PushFont(weight); }
        ~ScopedFont() { chai::ui::PopFont(); }
    };

    void Text(const std::string& label);
    void TextColored(const math::Vec4& color, const std::string& label);
    bool Button(const std::string& label);
    void SameLine();
    bool Checkbox(const std::string& label, bool& value);
    bool BeginChildRegion(const std::string& name,
                          const math::Vec2& size = math::Vec2(0, 0),
                          bool border = false);
    void EndChildRegion();
    float GetScrollY();
    float GetScrollMaxY();
    void SetScrollHereY(float y);
}