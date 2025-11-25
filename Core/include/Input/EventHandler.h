#pragma once
#include <cstdint>

namespace chai
{
    enum class InputEventType
    {
        KeyPress,
        KeyRelease,
        MouseMove,
        MouseButtonPress,
        MouseButtonRelease,
        MouseScroll,
        FramebufferResize,
        None
    };

    enum class KeyCode : uint32_t
    {
        W = 87, A = 65, S = 83, D = 68,
        Space = 32, C = 67,
        Escape = 256,
        // Add more later
    };

    enum class MouseButton : uint32_t
    {
        Left = 0, Right = 1, Middle = 2
    };

    class InputEvent
    {
    public:
        explicit InputEvent(uint64_t window) : windowId(window) { type = InputEventType::None; }

        uint64_t windowId;
        InputEventType type;
    };

    class KeyPressEvent : public InputEvent
    {
    public:
        KeyPressEvent(uint64_t window, int k) : InputEvent(window), key(k)
        {
            type = InputEventType::KeyPress;
        }

        int key;
    };

    class KeyReleaseEvent : public InputEvent
    {
    public:
        KeyReleaseEvent(uint64_t window, int k) : InputEvent(window), key(k)
        {
            type = InputEventType::KeyRelease;
        }

        int key;
    };

    class MouseDownEvent : public InputEvent
    {
    public:
        MouseDownEvent(uint64_t window, int b) : InputEvent(window), button(b)
        {
            type = InputEventType::MouseButtonPress;
        }

        uint32_t button;
    };

    class MouseUpEvent : public InputEvent
    {
    public:
        MouseUpEvent(uint64_t window, int b) : InputEvent(window), button(b)
        {
            type = InputEventType::MouseButtonRelease;
        }

        uint32_t button;
    };

    class MouseMoveEvent : public InputEvent
    {
    public:
        MouseMoveEvent(uint64_t window, double x, double y) : InputEvent(window), xPos(x), yPos(y)
        {
            type = InputEventType::MouseMove;
        }

        double xPos;
        double yPos;
    };

    class FrameBufferResize : public InputEvent
    {
    public:
        FrameBufferResize(uint64_t window, double w, double h) : InputEvent(window), width(w), height(h)
        {
            type = InputEventType::FramebufferResize;
        }

        double width;
        double height;
    };

    class IEventHandler
    {
    public:
        //only using a generic function for now, can be specialized later
        virtual ~IEventHandler() = default;

        virtual void handleEvent(const InputEvent& event)
        {
        }

        virtual void onKeyPress(const KeyPressEvent& event)
        {
        }
    };
}