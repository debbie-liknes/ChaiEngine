
#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>

namespace chai::scene
{
    CameraComponent::CameraComponent(GameObject* owner) : Component(owner)
    {
    }

    void CameraComponent::setAspectRatio(float aspectRatio)
    {
        cam_.setAspectRatio(aspectRatio);
    }

    void CameraComponent::setFarPlane(float far)
    {
        cam_.setFarPlane(far);
    }

    void CameraComponent::setNearPlane(float near)
    {
        cam_.setNearPlane(near);
    }

    void CameraComponent::setFOV(float fov)
    {
        cam_.setFOV(fov);
    }
}

CHAI_REFLECT(chai::scene::CameraComponent, "CameraComponent") {
    CHAI_ICON(ICON_FA_VIDEO);
}

