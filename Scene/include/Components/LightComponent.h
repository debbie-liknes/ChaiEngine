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
        LightComponent(GameObject* owner = nullptr);
        LightType type = LightType::DIRECTIONAL;
        Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
        float intensity = 0.7f;

		//Vec3 direction = Vec3(-1.0f, -1.0f, -1.0f);

        // For point/spot lights
        float range = 10.0f;
        Vec3 attenuation = Vec3(1.0f, 0.09f, 0.032f); // constant, linear, quadratic

        // For spot lights
        float innerCone = 12.5f; // degrees
        float outerCone = 17.5f; // degrees

        bool enabled = true;
    };
}