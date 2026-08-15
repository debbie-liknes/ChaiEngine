#pragma once
#include <ChaiMath.h>
#include <Components/Component.h>
#include <Scene/IUpdatable.h>

namespace chai::scene
{
    enum class LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    class GameObject;

    class LightComponent : public Component, public IUpdatable
    {
    public:
        explicit LightComponent(GameObject* owner = nullptr);

        LightType getType() const { return type_; }
        math::Vec3 getColor() const { return color_; }
        float getIntensity() const { return intensity_; }

    private:
        LightType type_ = LightType::DIRECTIONAL;
        math::Vec3 color_ = math::Vec3(3.f, 3.f, 3.f);
        float intensity_ = 1.4f;

        // For point/spot lights
        float range_ = 100.0f;
        math::Vec3 attenuation_ = math::Vec3(1.0f, 0.09f, 0.032f);

        // For spot lights
        float innerCone_ = 12.5f; // degrees
        float outerCone_ = 17.5f; // degrees

        bool enabled_ = true;
        bool shadowsEnabled_ = true;
    };

    CHAI_REFLECT(LightComponent, "LightComponent") 
    {
        CHAI_ICON(ICON_FA_LIGHTBULB);
    }
}