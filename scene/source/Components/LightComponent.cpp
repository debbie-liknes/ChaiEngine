#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    LightComponent::LightComponent(GameObject* owner) : Component(owner) {}
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
}

