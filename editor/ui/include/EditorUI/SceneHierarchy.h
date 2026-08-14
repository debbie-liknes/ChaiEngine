#pragma once
#include <vector>
#include <string>

namespace chai::scene
{
    class Scene;
}

namespace chai::ui
{
    struct SceneHierarchyNode {
        std::string name;
        std::vector<SceneHierarchyNode> children;
    };

    void drawSceneHierarchy(scene::Scene& scene);
}