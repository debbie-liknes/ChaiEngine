#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    LightComponent::LightComponent(GameObject* owner) : Component(owner) {}
} // namespace chai::scene