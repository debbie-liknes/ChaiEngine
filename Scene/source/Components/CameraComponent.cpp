#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>

namespace chai::cup
{
    CameraComponent::CameraComponent(GameObject* owner) : Component(owner)
    {
        m_camera = std::make_unique<Camera>();
    }

    Mat4 CameraComponent::getViewMatrix() const
    {
        TransformComponent const* transform = getGameObject()->getComponent<TransformComponent>();
        if (!transform) return Mat4{ 1.0f };

        // Build view matrix from transform
        Vec3 pos = transform->getWorldPosition();
        Vec3 forward = transform->forward();
        Vec3 up = transform->up();

        return lookAt(pos, pos + forward, up);
    }

    void CameraComponent::updateViewMatrix(TransformComponent* transform)
    {
        Vec3 position = transform->getWorldPosition();
        Vec3 forward = transform->forward();
        Vec3 up = transform->up();

        Mat4 viewMatrix = lookAt(position, position + forward, up);
        m_camera->setViewMatrix(viewMatrix);
    }

    void CameraComponent::update(double deltaTime)
    {
        if (auto owner = getGameObject(); owner)
        {
            updateViewMatrix(owner->getComponent<TransformComponent>());
        }
    }
}