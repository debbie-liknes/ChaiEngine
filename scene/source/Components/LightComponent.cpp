#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    LightComponent::LightComponent(GameObject* owner) : Component(owner) {}


    void LightComponent::setType(LightType type)
    {
        type_ = type;
    }

    void LightComponent::setColor(math::Vec3 color)
    {
        color_ = color;
    }

    void LightComponent::setIntensity(float intensity)
    {
        intensity_ = intensity;
    }
} // namespace chai::scene

CHAI_REFLECT(chai::scene::LightComponent, "LightComponent") {
    CHAI_FIELD(type_);
    CHAI_FIELD(color_);
    CHAI_FIELD(intensity_);
    CHAI_FIELD(range_);
    CHAI_FIELD(attenuation_);
    CHAI_FIELD(innerCone_);
    CHAI_FIELD(outerCone_);
    CHAI_FIELD(enabled_);
    CHAI_FIELD(shadowsEnabled_);

    CHAI_ICON(ICON_FA_LIGHTBULB);
    CHAI_PROPERTY("Intensity", getIntensity, setIntensity);
    CHAI_PROPERTY("Type", getType, setType);
    CHAI_PROPERTY("Color", getColor, setColor);
}

