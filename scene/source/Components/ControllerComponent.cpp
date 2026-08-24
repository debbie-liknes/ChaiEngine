#include <Components/ControllerComponent.h>

namespace chai::scene
{
    ControllerComponent::ControllerComponent(chai::scene::GameObject* owner) : Component(owner)
    {
    }
}

CHAI_REFLECT(chai::scene::ControllerComponent, "ControllerComponent") {
    CHAI_ICON(ICON_FA_GAMEPAD);
}

