#include <UI/Core/Container.h>

namespace chai::ui
{
    void Container::draw(UIContext& context)
    {
        for (auto& child : children_)
            child->draw(context);
    }
}