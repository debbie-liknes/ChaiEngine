#include <Components/CameraComponent.h>
#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>

namespace chai::cup
{
    CameraComponent::CameraComponent(GameObject* owner) : Component(owner)
    {
        m_camera = std::make_unique<Camera>();
    }

    glm::mat4 CameraComponent::getViewMatrix() const
    {
        TransformComponent const* transform = getGameObject()->getComponent<TransformComponent>();
        if (!transform) return glm::mat4(1.0f);

        // Build view matrix from transform
        glm::vec3 pos = transform->getWorldPosition();
        glm::vec3 forward = transform->forward();
        glm::vec3 up = transform->up();

        return glm::lookAt(pos, pos + forward, up);
    }

    void CameraComponent::updateViewMatrix(TransformComponent* transform)
    {
        glm::vec3 position = transform->getWorldPosition();
        glm::vec3 forward = transform->forward();
        glm::vec3 up = transform->up();

        glm::mat4 viewMatrix = glm::lookAt(position, position + forward, up);
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