#pragma once
#include <functional>
#include <queue>
#include <memory>
#include <Core/EventHandler.h>

namespace chai
{
    struct InputState
    {
        bool keys[256] = { false };
        float mouseX = 0.0f, mouseY = 0.0f;
        float lastMouseX = 0.0f, lastMouseY = 0.0f;
        bool firstMouse = true;
        bool mouseCaptured = false;
    };

    class InputSystem
    {
    public:
        using InputHandler = std::function<void(const InputEvent&)>;

        static InputSystem& instance();

        // Called by GLFW callbacks
        void queueEvent(std::unique_ptr<const InputEvent> event);

        // Subscribing
        uint32_t subscribe(InputHandler handler);
        void unsubscribe(uint32_t handlerId);

        // Process all queued events
        void processEvents();

        // state queries
        bool isKeyPressed(KeyCode key) const;
        bool isMouseButtonPressed(MouseButton button) const;
        void getMousePosition(float& x, float& y) const;
        void getMouseDelta(float& deltaX, float& deltaY) const;

        void clearFrame();

    private:
        InputSystem() = default;

        std::queue<std::unique_ptr<const InputEvent>> eventQueue;
        std::vector<std::pair<uint32_t, InputHandler>> handlers;
        uint32_t nextHandlerId = 1;

        // Current state for polling
        bool keyStates[512] = { false };
        bool mouseStates[8] = { false };
        float mouseX = 0.0f, mouseY = 0.0f;
        float mouseDeltaX = 0.0f, mouseDeltaY = 0.0f;

        void updateState(const InputEvent& event);
    };
}