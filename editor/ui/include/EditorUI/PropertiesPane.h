#pragma once
#include <string>
#include <vector>
#include <TypeInfo.h>

namespace chai::scene
{
    class Scene;
}

namespace chai::ui
{
    void drawPropertiesPane(void* obj, const TypeInfo& info);
} // namespace chai::ui