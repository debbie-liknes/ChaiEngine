#pragma once
#include <vector>
#include <string>
#include <functional>
#include <Scene/Object.h>

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

    void drawSceneHierarchy(scene::Scene& scene,
                            std::function<void(scene::ObjectId)> selectedCallback);
}