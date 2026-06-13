#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>

namespace chai::scene
{
    CameraComponent::CameraComponent(GameObject* owner) : IComponent(owner)
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

    void CameraComponent::extract(gfx::FrameRenderData& frame) const
    {
        auto const* t = getGameObject()->getComponent<TransformComponent>();
        const math::Mat4 world = t ? t->getWorldMatrix() : math::Mat4::identity();
        const math::Mat4 view = world.inverse();
        const math::Mat4 proj = cam_.getProjectionMatrix();
        frame.views.emplace_back(view, proj, proj * view, t->getWorldPosition());
    }
}