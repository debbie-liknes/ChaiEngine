#pragma once
#include <unordered_map>
#include <string>

namespace chai::ui
{
    struct AppFont;

    enum class FontWeight { Regular, Medium, Bold, Mono, Title, BoldTitle, Black };

    class FontManager
    {
    public:
        void loadFonts(const std::string& fontDirectory);
        void push(FontWeight weight) const;
        void pop() const;

    private:
        std::unordered_map<FontWeight, AppFont*> fonts_;
    };
}