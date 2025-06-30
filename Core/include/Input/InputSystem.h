#pragma once
#include <functional>
#include <memory>
#include <Input/InputState.h>
#include <queue>
#include <array>

namespace chai
{
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
        void updateState(const InputEvent& event);

    private:

        std::queue<std::unique_ptr<const InputEvent>> eventQueue;
        std::vector<std::pair<uint32_t, InputHandler>> handlers;
        uint32_t nextHandlerId = 1;

        // Current state for polling
        std::array<bool, 512> keyStates = { false };
        std::array<bool, 8> mouseStates = { false };
        double mouseX = 0.0f, mouseY = 0.0f;
        double mouseDeltaX = 0.0f, mouseDeltaY = 0.0f;

    };
}