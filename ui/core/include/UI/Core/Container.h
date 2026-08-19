#pragma once
#include <UI/Core/Widget.h>
#include <vector>
#include <memory>

namespace chai::ui
{
	class Container : public Widget
	{
    public:
        using Widget::Widget;

        template <typename T, typename... Args>
        T& add(Args&&... args)
        {
            auto widget = std::make_unique<T>(std::forward<Args>(args)...);

            T& result = *widget;
            children_.push_back(std::move(widget));

            return result;
        }

        void draw(UIContext& context) override;

    protected:
        std::vector<std::unique_ptr<Widget>> children_;
	};
}