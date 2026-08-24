#include <Components/SkyboxComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>
using namespace chai::gfx;

namespace chai::scene
{
    SkyboxComponent::SkyboxComponent(GameObject* owner) : Component(owner)
    {

    }

    SkyboxComponent ::~SkyboxComponent()
    {

    }
}

CHAI_REFLECT(chai::scene::SkyboxComponent, "SkyboxComponent") {
    CHAI_ICON(ICON_FA_CLOUD);
    CHAI_PROPERTY("Skybox", getTexture, setTexture);
}

