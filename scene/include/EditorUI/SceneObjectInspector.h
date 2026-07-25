#pragma once
#include <string>
#include <vector>

namespace chai::scene
{
    class GameObject;
}

namespace chai::ui
{
    void drawInspectorPanel(const scene::GameObject& object);
}