#pragma once
#include <UICoreExport.h>
#include <UI/Core/UIContext.h>

namespace chai::ui
{
    class UICORE_EXPORT Widget
    {
    public:
        virtual ~Widget() = default;

        virtual void draw(UIContext& context) = 0;

        bool visible = true;
    };
}