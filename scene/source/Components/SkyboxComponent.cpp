#include <Components/SkyboxComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>
using namespace chai::gfx;

namespace chai::scene
{
    SkyboxComponent::SkyboxComponent(GameObject* owner) : IComponent(owner)
    {

    }

    SkyboxComponent ::~SkyboxComponent()
    {

    }

    void SkyboxComponent::extract(gfx::FrameRenderData& frame) const
    {
        frame.environment = EnvironmentData{skyMap_};
    }
}