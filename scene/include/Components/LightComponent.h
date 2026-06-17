#pragma once
#include <ChaiMath.h>
#include <Components/Component.h>
#include <Updatable.h>

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

        virtual void extract(gfx::FrameRenderData& frame) const override;

    private:
        LightType type_ = LightType::DIRECTIONAL;
        math::Vec3 color_ = math::Vec3(1.0f, 1.0f, 1.0f);
        float intensity_ = 0.9f;

        // For point/spot lights
        float range_ = 100.0f;
        math::Vec3 attenuation_ = math::Vec3(1.0f, 0.09f, 0.032f);

        // For spot lights
        float innerCone_ = 12.5f; // degrees
        float outerCone_ = 17.5f; // degrees

        bool enabled_ = true;
        bool shadowsEnabled_ = true;
    };
}