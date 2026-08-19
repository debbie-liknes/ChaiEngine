#pragma once
#include <UICoreExport.h>
#include <string>
#include <UI/Core/Widget.h>
#include <functional>

namespace chai::ui
{
    struct ButtonStyle {
    };

	class Button : public Widget
	{
    public:
        explicit Button(const std::string& label);
        void draw(UIContext& context) override;

        std::function<void()> onClick;

    private:
        std::string label_;
	};

    bool checkbox(const std::string& label, bool* val);
} // namespace chai::ui