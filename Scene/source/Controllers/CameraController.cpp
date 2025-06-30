#include <Controllers/CameraController.h>
#include <stdexcept>
#include <cmath>

namespace chai::cup
{
    CameraController::CameraController(chai::cup::GameObject* obj) : IController(obj),
        cameraComponent(obj->getComponent<chai::cup::CameraComponent>()),
        transformComponent(obj->getComponent<chai::cup::TransformComponent>())
    {
        inputHandlerId = InputSystem::instance().subscribe(
            [this](const InputEvent& event) 
            {
                handleInput(event);
            }
        );

		glm::vec3 forward = transformComponent->forward();
        yaw = atan2(-forward.x, forward.z) * 180.0f / 3.14;
        pitch = asin(forward.y) * 180.0f / 3.14;
    }

    CameraController::~CameraController()
    {
        InputSystem::instance().unsubscribe(inputHandlerId);
    }

    void CameraController::handleInput(const InputEvent& event) 
    {
        if (event.type == InputEventType::MouseButtonPress) 
        {
            m_mouseCaptured = true;
        }
        else if (event.type == InputEventType::MouseButtonRelease) 
        {
            m_mouseCaptured = false;
        }
    }

    void CameraController::update(double deltaTime) 
    {
        processMovement(deltaTime);

        // Process mouse look if captured
        if (m_mouseCaptured) 
        {
            float deltaX, deltaY;
            InputSystem::instance().getMouseDelta(deltaX, deltaY);
            if (deltaX != 0.0f || deltaY != 0.0f) 
            {
                processMouseLook(deltaX, deltaY);
            }
        }
    }

    void CameraController::processMovement(float deltaTime) 
    {
        auto& input = InputSystem::instance();
        float velocity = moveSpeed * deltaTime;

        auto pos = transformComponent->getWorldPosition();
        auto forward = transformComponent->forward();
        auto right = transformComponent->right();

        if (input.isKeyPressed(KeyCode::W)) 
        {
            pos.x += forward.x * velocity;
            pos.y += forward.y * velocity;
            pos.z += forward.z * velocity;
        }
        if (input.isKeyPressed(KeyCode::S)) 
        {
            pos.x -= forward.x * velocity;
            pos.y -= forward.y * velocity;
            pos.z -= forward.z * velocity;
        }
        if (input.isKeyPressed(KeyCode::A)) 
        {
            pos.x -= right.x * velocity;
            pos.y -= right.y * velocity;
            pos.z -= right.z * velocity;
        }
        if (input.isKeyPressed(KeyCode::D)) 
        {
            pos.x += right.x * velocity;
            pos.y += right.y * velocity;
            pos.z += right.z * velocity;
        }
        if (input.isKeyPressed(KeyCode::Space)) 
        {
            pos.y += velocity;
        }
        if (input.isKeyPressed(KeyCode::C)) 
        {
            pos.y -= velocity;
        }

        transformComponent->setPosition(pos);
    }

    void CameraController::processMouseLook(float deltaX, float deltaY) 
    {
        deltaX *= mouseSensitivity;
        deltaY *= mouseSensitivity;

        yaw += deltaX;
        pitch += deltaY;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Standard OpenGL camera forward calculation
        float x = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        float y = sin(glm::radians(pitch));
        float z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

        auto pos = transformComponent->getWorldPosition();
        transformComponent->lookAt({ pos.x + x, pos.y + y, pos.z + z }, { 0.0f, 1.0f, 0.0f });
    }
}