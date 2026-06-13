#include <Components/LightComponent.h>
#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>

namespace chai::scene
{
    LightComponent::LightComponent(GameObject* owner) : IComponent(owner)
    {
    }

    void LightComponent::extract(gfx::FrameRenderData& frame) const 
    {
        auto const* t = getGameObject()->getComponent<TransformComponent>();
        auto dir = t->forward();
        if (type_ == LightType::DIRECTIONAL) {
            frame.sun = {math::Vec4{-2, -2, 0, 1.0},
                         math::Vec4{color_.x, color_.y, color_.z, 1.0}};
        }
    }
}