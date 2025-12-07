#pragma once
#include <Components/ComponentBase.h>
#include <ChaiMath.h>

namespace chai::cup
{
    enum class LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    class GameObject;

    class LightComponent : public Component
    {
    public:
        explicit LightComponent(GameObject* owner = nullptr);
        LightType type = LightType::DIRECTIONAL;
        Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
        float intensity = 3.f;

        // For point/spot lights
        float range = 100.0f;
        Vec3 attenuation = Vec3(1.0f, 0.09f, 0.032f); // constant, linear, quadratic

        // For spot lights
        float innerCone = 12.5f; // degrees
        float outerCone = 17.5f; // degrees

        bool enabled = true;
        bool shadowsEnabled = true;


    };
}