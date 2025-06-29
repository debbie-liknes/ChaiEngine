#pragma once
#include <vector>
#include <memory>
#include <ChaiEngine/Light.h>

namespace chai::cup
{
    class Scene;
    class GameObject;

    class LightingSystem {
    private:
        std::vector<brew::Light> lights;
        static const int MAX_LIGHTS = 16;

    public:
        void collectLights(Scene* scene);
        const std::vector<brew::Light>& getLights() const { return lights; }
        int getLightCount() const { return static_cast<int>(lights.size()); }
    };
}