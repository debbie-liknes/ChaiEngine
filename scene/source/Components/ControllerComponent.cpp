#include <Components/ControllerComponent.h>

namespace chai::scene
{
    ControllerComponent::ControllerComponent(chai::scene::GameObject* owner) : Component(owner)
    {
    }
}

CHAI_REFLECT(chai::scene::ControllerComponent, "ControllerComponent") {
    CHAI_ICON(ICON_FA_GAMEPAD);
   //CHAI_FIELD(controllers_); // unique_ptr does not play nicely with CHAI_REFLECT
    CHAI_FIELD(controllersByType_);
    CHAI_FIELD(controllersByName_);
    CHAI_FIELD(owner_);
}

