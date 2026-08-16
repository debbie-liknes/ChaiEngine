#pragma once
#include <UICoreExport.h>
#include <ChaiMath.h>
#include <string>

namespace chai::ui
{
    struct UICORE_EXPORT Size {
        float w = 0, h = 0;
    };

    struct UICORE_EXPORT Rect {
        float x = 0, y = 0, w = 0, h = 0;
        math::Vec2 min() const { return {x, y}; }
        math::Vec2 max() const { return {x + w, y + h}; }
    };

    struct UICORE_EXPORT Measured {
        Size desired;
        Size minimum;
    };

    math::Vec2 UICORE_EXPORT calcText(const std::string& text);
} // namespace chai::ui