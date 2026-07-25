#pragma once
#include <vector>
#include <string>

namespace chai::scene
{
    class Scene;
}

namespace chai::ui
{
    void drawSceneHierarchy(scene::Scene& scene);
}