#pragma once
#include <SceneExport.h>
#include <Controllers/Controller.h>
#include <Components/CameraComponent.h>
#include <Input/InputSystem.h>

namespace chai::cup
{
    // Camera controller class
    class SCENE_EXPORT CameraController : public IController
    {
    public:
        explicit CameraController(chai::cup::GameObject* obj);
        ~CameraController() override;

        void handleInput(const InputEvent& event);
        void processMovement(double deltaTime);
        void processMouseLook(float deltaX, float deltaY);

        // IController interface
        void update(double deltaTime) override;

        void setEnabled(bool enabled) override {}

        bool isEnabled() const override { return enabled; }
        const char* getControllerType() const override { return "CameraController"; }

        // Camera-specific interface
        void setMoveSpeed(float speed) { moveSpeed = speed; }
        void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
        float getMoveSpeed() const { return moveSpeed; }
        float getMouseSensitivity() const { return mouseSensitivity; }

    private:
        chai::cup::CameraComponent* cameraComponent;
        chai::cup::TransformComponent* transformComponent;
        float moveSpeed = 5.0f;
        float mouseSensitivity = 0.1f;
        double yaw = -90.0f; // Start looking towards negative Z
        double pitch = 0.0f;
        uint32_t inputHandlerId;
        bool m_mouseCaptured = false;
    };
}