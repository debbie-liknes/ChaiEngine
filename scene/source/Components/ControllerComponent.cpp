#include <Components/ControllerComponent.h>

namespace chai::scene
{
    ControllerComponent::ControllerComponent(chai::scene::GameObject* owner) : owner_(owner)
    {
    }
}

CHAI_REFLECT(chai::scene::ControllerComponent, "ControllerComponent") {}

